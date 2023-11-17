#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"
#include "helpers/StringHelper.h"
#include "helpers/NumericHelper.h"
#include "messages/MqttSNSubscribe.h"
#include "messages/MqttSNSubAck.h"

namespace mqttsn {

Define_Module(MqttSNSubscriber);

using json = nlohmann::json;

void MqttSNSubscriber::initializeCustom()
{
    fillTopics();

    subscriptionInterval = par("subscriptionInterval");
    subscriptionEvent = new inet::ClockEvent("subscriptionTimer");

    unsubscriptionInterval = par("unsubscriptionInterval");
    unsubscriptionEvent = new inet::ClockEvent("unsubscriptionTimer");
}

bool MqttSNSubscriber::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    if (msg == subscriptionEvent) {
        handleSubscriptionEvent();
    }
    else if (msg == unsubscriptionEvent) {
        handleUnsubscriptionEvent();
    }
    else {
        return false;
    }

    return true;
}

void MqttSNSubscriber::scheduleActiveStateEventsCustom()
{
    topicIds.clear();
}

void MqttSNSubscriber::cancelActiveStateEventsCustom()
{
    cancelEvent(subscriptionEvent);
    cancelEvent(unsubscriptionEvent);
}

void MqttSNSubscriber::cancelActiveStateClockEventsCustom()
{
    cancelClockEvent(subscriptionEvent);
    cancelClockEvent(unsubscriptionEvent);
}

void MqttSNSubscriber::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    switch(msgType) {
        // packet types that are allowed only from the connected gateway
        case MsgType::SUBACK:
            if (!MqttSNClient::isConnectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

        default:
            break;
    }

    switch(msgType) {
        case MsgType::SUBACK:
            processSubAck(pk, srcAddress, srcPort);
            break;

        default:
            break;
    }
}

void MqttSNSubscriber::processConnAckCustom()
{
    scheduleClockEventAfter(subscriptionInterval, subscriptionEvent);
    scheduleClockEventAfter(unsubscriptionInterval, unsubscriptionEvent);
}

void MqttSNSubscriber::processSubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNSubAck>();

    // check if the ACK is correct; exit if not
    if (!MqttSNClient::processAckForMsgType(MsgType::SUBSCRIBE, payload->getMsgId())) {
        return;
    }

    // now process and analyze message content as needed
    ReturnCode returnCode = payload->getReturnCode();

    if (returnCode == ReturnCode::REJECTED_CONGESTION) {
        lastSubscription.retry = true;
        scheduleClockEventAfter(MqttSNClient::waitingInterval, subscriptionEvent);
        return;
    }

    if (returnCode == ReturnCode::REJECTED_NOT_SUPPORTED) {
        lastSubscription.retry = false;
        scheduleClockEventAfter(MqttSNClient::waitingInterval, subscriptionEvent);
        return;
    }

    uint16_t topicId = payload->getTopicId();

    if (returnCode != ReturnCode::ACCEPTED || topicId == 0) {
        throw omnetpp::cRuntimeError("Unexpected error: Invalid return code or topic ID");
    }

    // handle operations when the subscription is ACCEPTED
    if (topicIds.find(topicId) == topicIds.end()) {
        // update only if the topic ID is new
        topicIds[topicId] = lastSubscription.info;
        NumericHelper::incrementCounter(&topics[lastSubscription.info.topicsKey].subscribeCounter);
    }

    lastSubscription.retry = false;
    scheduleClockEventAfter(subscriptionInterval, subscriptionEvent);
}

void MqttSNSubscriber::sendSubscribe(const inet::L3Address& destAddress, const int& destPort,
                                     bool dupFlag, QoS qosFlag, TopicIdType topicIdTypeFlag,
                                     uint16_t msgId,
                                     const std::string& topicName, uint16_t topicId)
{
    const auto& payload = inet::makeShared<MqttSNSubscribe>();
    payload->setMsgType(MsgType::SUBSCRIBE);
    payload->setDupFlag(dupFlag);
    payload->setQoSFlag(qosFlag);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setMsgId(msgId);

    if (!topicName.empty()) {
        payload->setTopicName(topicName);
    }
    if (topicId > 0) {
        payload->setTopicId(topicId);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("SubscribePacket");
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSessionFlag"), MqttSNClient::keepAlive);
}

void MqttSNSubscriber::handleSubscriptionEvent()
{
    std::string topicName;
    QoS qosFlag;

    // if it's a retry, use the last sent element
    if (lastSubscription.retry) {
        topicName = lastSubscription.info.topicName;
        qosFlag = topics[lastSubscription.info.topicsKey].qosFlag;
    }
    else {
        // check for topics availability
        if (topics.empty()) {
            throw omnetpp::cRuntimeError("No topic available");
        }

        // randomly select an element from the map
        auto it = topics.begin();
        std::advance(it, intuniform(0, topics.size() - 1));

        topicName = StringHelper::appendCounterToString(it->second.topicName, it->second.subscribeCounter);
        qosFlag = it->second.qosFlag;

        // update information about the last element
        lastSubscription.info.topicName = topicName;
        lastSubscription.info.topicsKey = it->first;
        lastSubscription.retry = true;
    }

    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port,
                  false, qosFlag, TopicIdType::NORMAL_TOPIC,
                  MqttSNClient::getNewMsgId(),
                  topicName, 0);

    // schedule subscribe retransmission
    std::map<std::string, std::string> parameters;
    parameters["msgId"] = std::to_string(MqttSNApp::currentMsgId);
    MqttSNClient::scheduleMsgRetransmission(
            MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, MsgType::SUBSCRIBE, &parameters
    );
}

void MqttSNSubscriber::handleUnsubscriptionEvent()
{
    // TO DO
}

void MqttSNSubscriber::fillTopics()
{
    json jsonData = json::parse(par("topicsJson").stringValue());
    int topicsKey = 0;

    // iterate over json object keys (topics) and fill the data structure
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        Topic topic;
        topic.topicName = it.key();
        topic.qosFlag = ConversionHelper::intToQoS(it.value()["qos"]);

        topics[topicsKey++] = topic;
    }
}

void MqttSNSubscriber::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                       omnetpp::cMessage* msg, MsgType msgType)
{
    switch (msgType) {
        case MsgType::SUBSCRIBE:
            retransmitSubscribe(destAddress, destPort, msg);
            break;

        default:
            break;
    }
}

void MqttSNSubscriber::retransmitSubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port,
                  true, topics[lastSubscription.info.topicsKey].qosFlag, TopicIdType::NORMAL_TOPIC,
                  std::stoi(msg->par("msgId").stringValue()),
                  lastSubscription.info.topicName, 0);
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
    cancelAndDelete(unsubscriptionEvent);
}

} /* namespace mqttsn */
