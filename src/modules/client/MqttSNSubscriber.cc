#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"
#include "helpers/StringHelper.h"
#include "helpers/NumericHelper.h"
#include "helpers/PacketHelper.h"
#include "messages/MqttSNSubscribe.h"
#include "messages/MqttSNSubAck.h"
#include "messages/MqttSNUnsubscribe.h"
#include "messages/MqttSNBaseWithMsgId.h"
#include "messages/MqttSNPublish.h"

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
        case MsgType::UNSUBACK:
        case MsgType::PUBLISH:
        case MsgType::PUBREL:
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

        case MsgType::UNSUBACK:
            processUnsubAck(pk, srcAddress, srcPort);
            break;

        case MsgType::PUBLISH:
            processPublish(pk, srcAddress, srcPort);
            break;

        case MsgType::PUBREL:
            processPubRel(pk, srcAddress, srcPort);
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

    // handle operations when the subscription is ACCEPTED; update data structures
    topicIds[topicId] = lastSubscription.info;
    NumericHelper::incrementCounter(&topics[lastSubscription.info.topicsKey].subscribeCounter);

    lastSubscription.retry = false;
    scheduleClockEventAfter(subscriptionInterval, subscriptionEvent);
}

void MqttSNSubscriber::processUnsubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithMsgId>();

    // check if the ACK is correct; exit if not
    if (!MqttSNClient::processAckForMsgType(MsgType::UNSUBSCRIBE, payload->getMsgId())) {
        return;
    }

    NumericHelper::incrementCounter(&topics[lastUnsubscription.info.topicsKey].unsubscribeCounter);

    lastUnsubscription.retry = false;
    scheduleClockEventAfter(unsubscriptionInterval, unsubscriptionEvent);
}

void MqttSNSubscriber::processPublish(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNPublish>();
    uint16_t topicId = payload->getTopicId();
    uint16_t msgId = payload->getMsgId();

    // check if the topic ID is present; if not, send a return code
    if (topicIds.find(topicId) == topicIds.end()) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::REJECTED_INVALID_TOPIC_ID, topicId, msgId);
        return;
    }

    QoS qosFlag = (QoS) payload->getQoSFlag();
    std::string data = payload->getData();

    MessageInfo messageInfo;
    messageInfo.dup = payload->getDupFlag();
    messageInfo.qos = qosFlag;
    messageInfo.topicId = topicId;
    messageInfo.topicName = topicIds[topicId].topicName;
    messageInfo.data = data;

    if (qosFlag == QoS::QOS_ZERO) {
        // handling QoS 0
        printPublishMessage(messageInfo);
        return;
    }

    // message ID check needed for QoS 1 and QoS 2
    if (msgId == 0) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::REJECTED_NOT_SUPPORTED, topicId, msgId);
        return;
    }

    if (qosFlag == QoS::QOS_ONE) {
        // handling QoS 1
        printPublishMessage(messageInfo);
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::ACCEPTED, topicId, msgId);
        return;
    }

    // handling QoS 2
    DataInfo dataInfo;
    dataInfo.topicId = topicId;
    dataInfo.topicName = topicIds[topicId].topicName;
    dataInfo.data = data;

    // save message data for reuse
    messages[msgId] = dataInfo;

    // send publish received
    sendBaseWithMsgId(srcAddress, srcPort, MsgType::PUBREC, msgId);
}

void MqttSNSubscriber::processPubRel(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithMsgId>();
    uint16_t msgId = payload->getMsgId();

    // check if the message exists for the given message ID
    auto messageIt = messages.find(msgId);
    if (messageIt != messages.end()) {
        // process the original publish message only once; as required for QoS 2 level
        const DataInfo& dataInfo = messageIt->second;

        MessageInfo messageInfo;
        messageInfo.dup = false;
        messageInfo.qos = QoS::QOS_TWO;
        messageInfo.topicId = dataInfo.topicId;
        messageInfo.topicName = dataInfo.topicName;
        messageInfo.data = dataInfo.data;

        // handling QoS 2
        printPublishMessage(messageInfo);

        // after processing, delete the message from the map
        messages.erase(messageIt);
    }

    // send publish complete
    sendBaseWithMsgId(srcAddress, srcPort, MsgType::PUBCOMP, msgId);
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

void MqttSNSubscriber::sendUnsubscribe(const inet::L3Address& destAddress, const int& destPort,
                                       TopicIdType topicIdTypeFlag,
                                       uint16_t msgId,
                                       const std::string& topicName, uint16_t topicId)
{
    const auto& payload = inet::makeShared<MqttSNUnsubscribe>();
    payload->setMsgType(MsgType::UNSUBSCRIBE);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setMsgId(msgId);

    if (!topicName.empty()) {
        payload->setTopicName(topicName);
    }
    if (topicId > 0) {
        payload->setTopicId(topicId);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("UnsubscribePacket");
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort,
                                                MsgType msgType, ReturnCode returnCode,
                                                uint16_t topicId, uint16_t msgId)
{
    MqttSNApp::socket.sendTo(PacketHelper::getMsgIdWithTopicIdPlusPacket(msgType, returnCode, topicId, msgId),
                             destAddress,
                             destPort);
}

void MqttSNSubscriber::sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId)
{
    MqttSNApp::socket.sendTo(PacketHelper::getBaseWithMsgIdPacket(msgType, msgId), destAddress, destPort);
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
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::SUBSCRIBE, MqttSNClient::currentMsgId);
}

void MqttSNSubscriber::handleUnsubscriptionEvent()
{
    std::string topicName;

    // if it's a retry, use the last sent element
    if (lastUnsubscription.retry) {
        topicName = lastUnsubscription.info.topicName;
    }
    else {
        // check for topics availability
        if (topics.empty()) {
            throw omnetpp::cRuntimeError("No topic available");
        }

        // randomly select an element from the map
        auto it = topics.begin();
        std::advance(it, intuniform(0, topics.size() - 1));

        // unsubcription must be after subscription
        if (it->second.unsubscribeCounter == it->second.subscribeCounter) {
            scheduleClockEventAfter(unsubscriptionInterval, unsubscriptionEvent);
            return;
        }

        topicName = StringHelper::appendCounterToString(it->second.topicName, it->second.unsubscribeCounter);

        // update information about the last element
        lastUnsubscription.info.topicName = topicName;
        lastUnsubscription.info.topicsKey = it->first;
        lastUnsubscription.retry = true;
    }

    sendUnsubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port,
                    TopicIdType::NORMAL_TOPIC,
                    MqttSNClient::getNewMsgId(),
                    topicName, 0);

    // schedule unsubscribe retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::UNSUBSCRIBE, MqttSNClient::currentMsgId);
}

void MqttSNSubscriber::fillTopics()
{
    json jsonData = json::parse(par("topicsJson").stringValue());
    int topicsKey = 0;

    // iterate over json object keys (topics)
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        Topic topic;
        topic.topicName = it.key();
        topic.qosFlag = ConversionHelper::intToQoS(it.value()["qos"]);

        topics[topicsKey++] = topic;
    }
}

void MqttSNSubscriber::printPublishMessage(const MessageInfo& messageInfo)
{
    EV << "Received publish message:" << std::endl;
    EV << "Duplicate flag: " << (messageInfo.dup ? "True" : "False") << std::endl;
    EV << "QoS: " << (int) messageInfo.qos << std::endl;
    EV << "Topic ID: " << messageInfo.topicId << std::endl;
    EV << "Topic name: " << messageInfo.topicName << std::endl;
    EV << "Data: " << messageInfo.data << std::endl;
}

void MqttSNSubscriber::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                       omnetpp::cMessage* msg, MsgType msgType)
{
    switch (msgType) {
        case MsgType::SUBSCRIBE:
            retransmitSubscribe(destAddress, destPort, msg);
            break;

        case MsgType::UNSUBSCRIBE:
            retransmitUnsubscribe(destAddress, destPort, msg);
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

void MqttSNSubscriber::retransmitUnsubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendUnsubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port,
                  TopicIdType::NORMAL_TOPIC,
                  std::stoi(msg->par("msgId").stringValue()),
                  lastUnsubscription.info.topicName, 0);
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
    cancelAndDelete(unsubscriptionEvent);
}

} /* namespace mqttsn */
