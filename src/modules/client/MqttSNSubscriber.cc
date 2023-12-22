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

void MqttSNSubscriber::levelTwoInit()
{
    populateItems();

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
    resetAndPopulateTopics();
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
    TopicInfo& topicInfo = topics[topicId];
    topicInfo.topicName = lastSubscription.topicName;
    topicInfo.itemInfo = lastSubscription.itemInfo;

    NumericHelper::incrementCounter(&(lastSubscription.itemInfo->subscribeCounter));

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

    auto& itemInfo = lastUnsubscription.itemInfo;
    TopicIdType topicIdType = itemInfo->topicIdType;

    // enable re-subscription for predefined/short topics
    if (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID ||
        topicIdType == TopicIdType::SHORT_TOPIC_ID) {

        itemInfo->subscribeCounter = 0;
    }
    else {
        NumericHelper::incrementCounter(&(itemInfo->unsubscribeCounter));
    }

    lastUnsubscription.retry = false;
    scheduleClockEventAfter(unsubscriptionInterval, unsubscriptionEvent);
}

void MqttSNSubscriber::processPublish(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNPublish>();
    uint16_t topicId = payload->getTopicId();
    TopicIdType topicIdType = (TopicIdType) payload->getTopicIdTypeFlag();
    uint16_t msgId = payload->getMsgId();

    // verify topic ID existence and type consistency
    auto it = topics.find(topicId);
    if (it == topics.end() || it->second.itemInfo->topicIdType != topicIdType) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::REJECTED_INVALID_TOPIC_ID, topicId, msgId);
        return;
    }

    QoS qos = (QoS) payload->getQoSFlag();
    bool retain = payload->getRetainFlag();
    std::string data = payload->getData();

    MessageInfo messageInfo;
    messageInfo.topicName = topics[topicId].topicName;
    messageInfo.topicId = topicId;
    messageInfo.topicIdType = topicIdType;
    messageInfo.dup = payload->getDupFlag();
    messageInfo.qos = qos;
    messageInfo.retain = retain;
    messageInfo.data = data;

    if (qos == QoS::QOS_ZERO) {
        // handling QoS 0
        printPublishMessage(messageInfo);
        return;
    }

    // message ID check needed for QoS 1 and QoS 2
    if (msgId == 0) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::REJECTED_NOT_SUPPORTED, topicId, msgId);
        return;
    }

    if (qos == QoS::QOS_ONE) {
        // handling QoS 1
        printPublishMessage(messageInfo);
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, ReturnCode::ACCEPTED, topicId, msgId);
        return;
    }

    // handling QoS 2
    DataInfo dataInfo;
    dataInfo.topicName = topics[topicId].topicName;
    dataInfo.topicId = topicId;
    dataInfo.topicIdType = topicIdType;
    dataInfo.retain = retain;
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
        messageInfo.topicName = dataInfo.topicName;
        messageInfo.topicId = dataInfo.topicId;
        messageInfo.topicIdType = dataInfo.topicIdType;
        messageInfo.dup = false;
        messageInfo.qos = QoS::QOS_TWO;
        messageInfo.retain = dataInfo.retain;
        messageInfo.data = dataInfo.data;

        // handling QoS 2
        printPublishMessage(messageInfo);

        // after processing, delete the message from the map
        messages.erase(messageIt);
    }

    // send publish complete
    sendBaseWithMsgId(srcAddress, srcPort, MsgType::PUBCOMP, msgId);
}

void MqttSNSubscriber::sendSubscribe(const inet::L3Address& destAddress, const int& destPort, bool dupFlag, QoS qosFlag,
                                     TopicIdType topicIdTypeFlag, uint16_t msgId, const std::string& topicName, uint16_t topicId,
                                     bool useTopicId)
{
    validateTopic(topicName, topicId, useTopicId);

    const auto& payload = inet::makeShared<MqttSNSubscribe>();
    payload->setMsgType(MsgType::SUBSCRIBE);
    payload->setDupFlag(dupFlag);
    payload->setQoSFlag(qosFlag);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setMsgId(msgId);

    if (useTopicId) {
        payload->setTopicId(topicId);
    }
    else {
        payload->setTopicName(topicName);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("SubscribePacket");
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::sendUnsubscribe(const inet::L3Address& destAddress, const int& destPort, TopicIdType topicIdTypeFlag, uint16_t msgId,
                                       const std::string& topicName, uint16_t topicId, bool useTopicId)
{
    validateTopic(topicName, topicId, useTopicId);

    const auto& payload = inet::makeShared<MqttSNUnsubscribe>();
    payload->setMsgType(MsgType::UNSUBSCRIBE);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setMsgId(msgId);

    if (useTopicId) {
        payload->setTopicId(topicId);
    }
    else {
        payload->setTopicName(topicName);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("UnsubscribePacket");
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, ReturnCode returnCode,
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
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSession"), MqttSNClient::keepAlive);
}

void MqttSNSubscriber::handleSubscriptionEvent()
{
    if (!proceedWithSubscription()) {
        return;
    }

    TopicIdType topicIdType = lastSubscription.itemInfo->topicIdType;

    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, false, lastSubscription.itemInfo->qos,
                  topicIdType, MqttSNClient::getNewMsgId(), lastSubscription.topicName, lastSubscription.itemInfo->topicId,
                  (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));

    // schedule subscribe retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::SUBSCRIBE, MqttSNClient::currentMsgId);
}

void MqttSNSubscriber::handleUnsubscriptionEvent()
{
    if (!proceedWithUnsubscription()) {
        return;
    }

    TopicIdType topicIdType = lastUnsubscription.itemInfo->topicIdType;

    sendUnsubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, topicIdType, MqttSNClient::getNewMsgId(),
                    lastUnsubscription.topicName, lastUnsubscription.itemInfo->topicId, (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));

    // schedule unsubscribe retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::UNSUBSCRIBE, MqttSNClient::currentMsgId);
}

void MqttSNSubscriber::populateItems()
{
    json jsonData = json::parse(par("itemsJson").stringValue());
    int itemsKey = 0;

    // iterate over json array elements
    for (const auto& item : jsonData) {
        std::string topicName = item["topic"];
        TopicIdType topicIdType = ConversionHelper::stringToTopicIdType(item["idType"]);

        // validate topic name length and type against specified criteria
        MqttSNApp::checkTopicLength(topicName.length(), topicIdType);

        auto predefinedTopicIt = MqttSNClient::predefinedTopics.find(StringHelper::base64Encode(topicName));

        // validate topic consistency
        MqttSNClient::checkTopicConsistency(
                topicName, topicIdType,
                predefinedTopicIt != MqttSNClient::predefinedTopics.end()
        );

        ItemInfo itemInfo;
        itemInfo.topicName = topicName;
        itemInfo.topicIdType = topicIdType;
        itemInfo.topicId = predefinedTopicIt->second;
        itemInfo.qos = ConversionHelper::intToQoS(item["qos"]);

        items[itemsKey++] = itemInfo;
    }
}

void MqttSNSubscriber::resetAndPopulateTopics()
{
    topics.clear();

    for (auto& pair : items) {
        ItemInfo& itemInfo = pair.second;

        // reset the counters if the topic uses a short ID type
        if (itemInfo.topicIdType == TopicIdType::SHORT_TOPIC_ID) {
            itemInfo.subscribeCounter = 0;
            itemInfo.unsubscribeCounter = 0;
        }

        // insert the predefined topics
        if (itemInfo.topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID) {
            TopicInfo topicInfo;
            topicInfo.topicName = itemInfo.topicName;
            topicInfo.itemInfo = &itemInfo;

            topics[MqttSNClient::getPredefinedTopicId(itemInfo.topicName)] = topicInfo;
        }
    }
}

void MqttSNSubscriber::validateTopic(const std::string& topicName, uint16_t topicId, bool useTopicId)
{
    // check for a valid topic ID or a valid topic name
    if ((useTopicId && topicId == 0) || (!useTopicId && topicName.empty())) {
        throw omnetpp::cRuntimeError(useTopicId ? "Topic ID must be specified" : "Topic name must not be empty");
    }
}

bool MqttSNSubscriber::proceedWithSubscription()
{
    // if it's a retry, use the last sent element
    if (lastSubscription.retry) {
        return true;
    }

    // check for items availability
    if (items.empty()) {
        throw omnetpp::cRuntimeError("No item available");
    }

    // randomly select an element from the map
    auto it = items.begin();
    std::advance(it, intuniform(0, items.size() - 1));

    TopicIdType topicIdType = it->second.topicIdType;
    int subscribeCounter = it->second.subscribeCounter;

    // subscribe predefined/short topics once: initially or post-unsubscribe
    if ((topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID || topicIdType == TopicIdType::SHORT_TOPIC_ID) &&
         subscribeCounter == 1) {

        scheduleClockEventAfter(MqttSNClient::MIN_WAITING_TIME, subscriptionEvent);
        return false;
    }

    // update information about the last element
    lastSubscription.topicName = StringHelper::appendCounterToString(it->second.topicName, subscribeCounter);
    lastSubscription.itemInfo = &it->second;
    lastSubscription.retry = true;

    return true;
}

bool MqttSNSubscriber::proceedWithUnsubscription()
{
    // if it's a retry, use the last sent element
    if (lastUnsubscription.retry) {
        return true;
    }

    // check for items availability
    if (items.empty()) {
        throw omnetpp::cRuntimeError("No item available");
    }

    // randomly select an element from the map
    auto it = items.begin();
    std::advance(it, intuniform(0, items.size() - 1));

    int unsubscribeCounter = it->second.unsubscribeCounter;

    // unsubcription must be after subscription
    if (unsubscribeCounter == it->second.subscribeCounter) {
        scheduleClockEventAfter(MqttSNClient::MIN_WAITING_TIME, unsubscriptionEvent);
        return false;
    }

    // update information about the last element
    lastUnsubscription.topicName = StringHelper::appendCounterToString(it->second.topicName, unsubscribeCounter);
    lastUnsubscription.itemInfo = &it->second;
    lastUnsubscription.retry = true;

    return true;
}

void MqttSNSubscriber::printPublishMessage(const MessageInfo& messageInfo)
{
    EV << "Received publish message:" << std::endl;
    EV << "Topic name: " << messageInfo.topicName << std::endl;
    EV << "Topic ID: " << messageInfo.topicId << std::endl;
    EV << "Topic ID type: " << ConversionHelper::topicIdTypeToString(messageInfo.topicIdType) << std::endl;
    EV << "Duplicate: " << (messageInfo.dup ? "True" : "False") << std::endl;
    EV << "QoS: " << ConversionHelper::qosToInt(messageInfo.qos) << std::endl;
    EV << "Retain: " << messageInfo.retain << std::endl;
    EV << "Data: " << messageInfo.data << std::endl;
}

void MqttSNSubscriber::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg,
                                                       MsgType msgType)
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
    TopicIdType topicIdType = lastSubscription.itemInfo->topicIdType;

    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, true, lastSubscription.itemInfo->qos,
                  topicIdType, std::stoi(msg->par("msgId").stringValue()), lastSubscription.topicName, lastSubscription.itemInfo->topicId,
                  (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));
}

void MqttSNSubscriber::retransmitUnsubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    TopicIdType topicIdType = lastUnsubscription.itemInfo->topicIdType;

    sendUnsubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, topicIdType,
                    std::stoi(msg->par("msgId").stringValue()), lastUnsubscription.topicName, lastUnsubscription.itemInfo->topicId,
                    (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
    cancelAndDelete(unsubscriptionEvent);
}

} /* namespace mqttsn */
