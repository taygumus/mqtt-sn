#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"
#include "helpers/StringHelper.h"
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
}

bool MqttSNSubscriber::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    if (msg == subscriptionEvent) {
        handleSubscriptionEvent();
    }
    else {
        return false;
    }

    return true;
}

void MqttSNSubscriber::scheduleActiveStateEventsCustom()
{
    //
}

void MqttSNSubscriber::cancelActiveStateEventsCustom()
{
    cancelEvent(subscriptionEvent);
}

void MqttSNSubscriber::cancelActiveStateClockEventsCustom()
{
    cancelClockEvent(subscriptionEvent);
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
}

void MqttSNSubscriber::processSubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNSubAck>();

    // now process and analyze message content as needed
    ReturnCode returnCode = payload->getReturnCode();

    // TO DO
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
    // check for topics availability
    if (topics.empty()) {
        throw omnetpp::cRuntimeError("No topic available");
    }

    // randomly select an element from the map
    auto it = topics.begin();
    std::advance(it, intuniform(0, topics.size() - 1));

    std::string topicName = StringHelper::appendCounterToString(it->second.topicName, it->second.subscribeCounter);

    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port,
                  false, it->second.qosFlag, TopicIdType::NORMAL_TOPIC,
                  MqttSNClient::getNewMsgId(),
                  topicName, 0);
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
    // TO DO
    switch (msgType) {
        //
        default:
            break;
    }
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
}

} /* namespace mqttsn */
