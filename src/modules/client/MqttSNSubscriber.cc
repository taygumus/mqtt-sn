#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "inet/common/TimeTag_m.h"
#include "tags/IdentifierTag.h"
#include "helpers/ConversionHelper.h"
#include "helpers/StringHelper.h"
#include "helpers/NumericHelper.h"
#include "helpers/PacketHelper.h"
#include "messages/MqttSNSubscribe.h"
#include "messages/MqttSNSubAck.h"
#include "messages/MqttSNUnsubscribe.h"
#include "messages/MqttSNBaseWithMsgId.h"
#include "messages/MqttSNRegister.h"
#include "messages/MqttSNPublish.h"

namespace mqttsn {

Define_Module(MqttSNSubscriber);

using json = nlohmann::json;

std::set<unsigned> MqttSNSubscriber::publishMsgIdentifiers;

void MqttSNSubscriber::levelTwoInit()
{
    populateItems();

    subscriptionInterval = par("subscriptionInterval");
    subscriptionEvent = new inet::ClockEvent("subscriptionTimer");

    unsubscriptionInterval = par("unsubscriptionInterval");
    unsubscriptionEvent = new inet::ClockEvent("unsubscriptionTimer");

    instancePublishMsgIdentifiers.clear();
    publishMsgIdentifiers.clear();
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
    // reset last operations
    lastSubscription.retry = false;
    lastUnsubscription.retry = false;

    // reset counters
    subscriptionCounter = 0;
    unsubscriptionCounter = 0;

    // reset and initialize topics
    resetAndPopulateTopics();

    // reset messages, if any
    messages.clear();
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

void MqttSNSubscriber::adjustAllowedPacketTypes(std::vector<MsgType>& msgTypes)
{
    msgTypes.push_back(MsgType::PUBLISH);
    msgTypes.push_back(MsgType::PUBREL);
}

void MqttSNSubscriber::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    switch(msgType) {
        // packet types that are allowed only from the connected gateway
        case MsgType::SUBACK:
        case MsgType::UNSUBACK:
        case MsgType::REGISTER:
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

        case MsgType::REGISTER:
            processRegister(pk, srcAddress, srcPort);
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

    subscriptionCounter++;

    EV << "Subscription completed - Topic Name: " << lastSubscription.topicName << std::endl;
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
    if (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID || topicIdType == TopicIdType::SHORT_TOPIC_ID) {
        itemInfo->subscribeCounter = 0;
    }
    else {
        NumericHelper::incrementCounter(&(itemInfo->unsubscribeCounter));
    }

    lastUnsubscription.retry = false;
    scheduleClockEventAfter(unsubscriptionInterval, unsubscriptionEvent);

    unsubscriptionCounter++;

    EV << "Unsubscription completed - Topic Name: " << lastUnsubscription.topicName << std::endl;
}

void MqttSNSubscriber::processRegister(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNRegister>();
    uint16_t topicId = payload->getTopicId();
    uint16_t msgId = payload->getMsgId();

    // reject registration if the topic ID is zero
    if (topicId == 0) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::REGACK, topicId, msgId, ReturnCode::REJECTED_INVALID_TOPIC_ID);
        return;
    }

    // check if the topic is already registered; if yes, send ACCEPTED response, otherwise register the topic
    auto topicIt = topics.find(topicId);
    if (topicIt != topics.end()) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::REGACK, topicId, msgId, ReturnCode::ACCEPTED);
        return;
    }

    // extract and sanitize the topic name from the payload
    std::string topicName = StringHelper::sanitizeSpaces(payload->getTopicName());

    // reject registration if the topic name length is less than the minimum required
    if (!MqttSNApp::isMinTopicLength(topicName.length())) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::REGACK, topicId, msgId, ReturnCode::REJECTED_NOT_SUPPORTED);
        return;
    }

    // find the item by the base topic name
    ItemInfo* itemInfo = findItemByTopicName(
            StringHelper::getStringBeforeDelimiter(topicName, MqttSNClient::TOPIC_DELIMITER)
    );

    // reject registration if the associated item is not found
    if (itemInfo == nullptr) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::REGACK, topicId, msgId, ReturnCode::REJECTED_NOT_SUPPORTED);
        return;
    }

    addNewTopic(topicId, topicName, itemInfo);

    // send REGACK response with the new topic ID and ACCEPTED status
    sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::REGACK, topicId, msgId, ReturnCode::ACCEPTED);
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
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, topicId, msgId, ReturnCode::REJECTED_INVALID_TOPIC_ID);
        return;
    }

    QoS qos = (QoS) payload->getQoSFlag();
    bool retain = payload->getRetainFlag();
    std::string data = payload->getData();

    TagInfo tagInfo;
    tagInfo.timestamp = inet::ClockTime::SIMTIME_AS_CLOCKTIME(payload->findTag<inet::CreationTimeTag>()->getCreationTime());
    tagInfo.identifier = payload->findTag<IdentifierTag>()->getIdentifier();

    MessageInfo messageInfo;
    messageInfo.topicName = topics[topicId].topicName;
    messageInfo.topicId = topicId;
    messageInfo.topicIdType = topicIdType;
    messageInfo.dup = payload->getDupFlag();
    messageInfo.qos = qos;
    messageInfo.retain = retain;
    messageInfo.data = data;
    messageInfo.tagInfo = tagInfo;

    if (qos == QoS::QOS_MINUS_ONE || qos == QoS::QOS_ZERO) {
        // handling QoS -1 or QoS 0
        printPublishMessage(messageInfo);
        handlePublishMessageMetrics(tagInfo);
        return;
    }

    if (qos == QoS::QOS_ONE) {
        // handling QoS 1
        printPublishMessage(messageInfo);
        handlePublishMessageMetrics(tagInfo);
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, topicId, msgId, ReturnCode::ACCEPTED);
        return;
    }

    // handling QoS 2; message ID check needed
    if (msgId == 0) {
        sendMsgIdWithTopicIdPlus(srcAddress, srcPort, MsgType::PUBACK, topicId, msgId, ReturnCode::REJECTED_NOT_SUPPORTED);
        return;
    }

    DataInfo dataInfo;
    dataInfo.topicName = topics[topicId].topicName;
    dataInfo.topicId = topicId;
    dataInfo.topicIdType = topicIdType;
    dataInfo.retain = retain;
    dataInfo.data = data;
    dataInfo.tagInfo = tagInfo;

    // save message data for reuse
    messages[msgId] = dataInfo;

    // send PUBlish RECeived
    sendBaseWithMsgId(srcAddress, srcPort, MsgType::PUBREC, msgId);
}

void MqttSNSubscriber::processPubRel(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithMsgId>();
    uint16_t msgId = payload->getMsgId();

    // check if the message exists for the given message ID
    auto messageIt = messages.find(msgId);
    if (messageIt != messages.end()) {
        // process the original PUBLISH message only once; as required for QoS 2
        const DataInfo& dataInfo = messageIt->second;

        MessageInfo messageInfo;
        messageInfo.topicName = dataInfo.topicName;
        messageInfo.topicId = dataInfo.topicId;
        messageInfo.topicIdType = dataInfo.topicIdType;
        messageInfo.dup = false;
        messageInfo.qos = QoS::QOS_TWO;
        messageInfo.retain = dataInfo.retain;
        messageInfo.data = dataInfo.data;
        messageInfo.tagInfo = dataInfo.tagInfo;

        // handling QoS 2
        printPublishMessage(messageInfo);
        handlePublishMessageMetrics(messageInfo.tagInfo);

        // after processing, delete the message from the map
        messages.erase(messageIt);
    }

    // send PUBlish COMPlete
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
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

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
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t topicId,
                                                uint16_t msgId, ReturnCode returnCode)
{
    inet::Packet* packet = PacketHelper::getMsgIdWithTopicIdPlusPacket(msgType, topicId, msgId, returnCode);
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNSubscriber::sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId)
{
    inet::Packet* packet = PacketHelper::getBaseWithMsgIdPacket(msgType, msgId);
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
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

    // schedule SUBSCRIBE retransmission
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

    // schedule UNSUBSCRIBE retransmission
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
        bool isPredefined = predefinedTopicIt != MqttSNClient::predefinedTopics.end();

        // validate topic consistency
        MqttSNClient::checkTopicConsistency(topicName, topicIdType, isPredefined);

        ItemInfo itemInfo;
        itemInfo.topicName = topicName;
        itemInfo.topicIdType = topicIdType;
        itemInfo.qos = ConversionHelper::intToQoS(item["qos"]);

        if (isPredefined) {
            itemInfo.topicId = predefinedTopicIt->second;
        }

        items[itemsKey++] = itemInfo;
    }
}

ItemInfo* MqttSNSubscriber::findItemByTopicName(const std::string& topicName)
{
    // search for an item with the provided topic name
    for (auto& item : items) {
        // check for a matching topic name
        if (item.second.topicName == topicName) {
            return &item.second;
        }
    }

    return nullptr;
}

void MqttSNSubscriber::resetAndPopulateTopics()
{
    topics.clear();

    for (auto& pair : items) {
        ItemInfo& itemInfo = pair.second;

        // insert the predefined topics
        if (itemInfo.topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID) {
            addNewTopic(MqttSNClient::getPredefinedTopicId(itemInfo.topicName), itemInfo.topicName, &itemInfo);
        }

        // reset the counters
        itemInfo.subscribeCounter = 0;
        itemInfo.unsubscribeCounter = 0;
    }
}

void MqttSNSubscriber::addNewTopic(uint16_t topicId, const std::string& topicName, ItemInfo* itemInfo)
{
    TopicInfo topicInfo;
    topicInfo.topicName = topicName;
    topicInfo.itemInfo = itemInfo;

    topics[topicId] = topicInfo;
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

    int subscriptionLimit = par("subscriptionLimit");
    // check if the subscription limit is set and reached
    if (subscriptionLimit != -1 && subscriptionLimit == subscriptionCounter) {
        return false;
    }

    // check for items availability
    if (items.empty()) {
        throw omnetpp::cRuntimeError("No item available");
    }

    auto itemIt = items.end();

    // search for the first item with counter zero
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->second.subscribeCounter == 0) {
            itemIt = it;
            break;
        }
    }

    // randomly select an item if all items are subscribed at least once
    if (itemIt == items.end()) {
        itemIt = items.begin();
        std::advance(itemIt, intuniform(0, items.size() - 1));
    }

    TopicIdType topicIdType = itemIt->second.topicIdType;
    int subscribeCounter = itemIt->second.subscribeCounter;

    // subscribe predefined/short topics once: initially or post-unsubscribe
    if ((topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID || topicIdType == TopicIdType::SHORT_TOPIC_ID) &&
         subscribeCounter == 1) {

        scheduleClockEventAfter(MqttSNClient::MIN_WAITING_TIME, subscriptionEvent);
        return false;
    }

    // update information about the last element
    lastSubscription.topicName = StringHelper::appendCounterToString(itemIt->second.topicName, MqttSNClient::TOPIC_DELIMITER, subscribeCounter);
    lastSubscription.itemInfo = &itemIt->second;

    return true;
}

bool MqttSNSubscriber::proceedWithUnsubscription()
{
    // if it's a retry, use the last sent element
    if (lastUnsubscription.retry) {
        return true;
    }

    int unsubscriptionLimit = par("unsubscriptionLimit");
    // check if the unsubscription limit is set and reached
    if (unsubscriptionLimit != -1 && unsubscriptionLimit == unsubscriptionCounter) {
        return false;
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
    lastUnsubscription.topicName = StringHelper::appendCounterToString(it->second.topicName, MqttSNClient::TOPIC_DELIMITER, unsubscribeCounter);
    lastUnsubscription.itemInfo = &it->second;

    return true;
}

void MqttSNSubscriber::printPublishMessage(const MessageInfo& messageInfo)
{
    EV << "Received publish message:" << std::endl;
    EV << "Topic name: " << messageInfo.topicName << std::endl;
    EV << "Topic ID: " << messageInfo.topicId << std::endl;
    EV << "Topic ID type: " << ConversionHelper::topicIdTypeToString(messageInfo.topicIdType) << std::endl;
    EV << "Duplicate: " << messageInfo.dup << std::endl;
    EV << "QoS: " << ConversionHelper::qosToInt(messageInfo.qos) << std::endl;
    EV << "Retain: " << messageInfo.retain << std::endl;
    EV << "Data: " << messageInfo.data << std::endl;
    EV << "Timestamp tag: " << messageInfo.tagInfo.timestamp << std::endl;
    EV << "ID tag: " << messageInfo.tagInfo.identifier << std::endl;
}

void MqttSNSubscriber::handlePublishMessageMetrics(const TagInfo& tagInfo)
{
    // return if the tag information is not valid
    if (tagInfo.timestamp == 0 || tagInfo.identifier == 0) {
        return;
    }

    // end-to-end delay in seconds of current message
    inet::clocktime_t endToEndDelay = getClockTime() - tagInfo.timestamp;

    // print the current message delay
    EV << "End-to-end delay: " << endToEndDelay << " seconds" << std::endl;

    MqttSNClient::sumReceivedPublishMsgTimestamps += endToEndDelay.dbl();
    MqttSNClient::receivedTotalPublishMsgs++;

    // duplicate detection
    auto identifierIt = instancePublishMsgIdentifiers.find(tagInfo.identifier);
    if (identifierIt == instancePublishMsgIdentifiers.end()) {
        // insert the message identifier into the instance set to track unique messages
        instancePublishMsgIdentifiers.insert(tagInfo.identifier);
    }
    else {
        // increment the count of duplicate PUBLISH messages received so far
        MqttSNClient::receivedDuplicatePublishMsgs++;
    }

    // insert the message identifier into the set to track unique messages
    publishMsgIdentifiers.insert(tagInfo.identifier);

    // count of unique PUBLISH messages received so far
    MqttSNClient::receivedUniquePublishMsgs = publishMsgIdentifiers.size();
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

void MqttSNSubscriber::updateRetransmissionsCounter()
{
    MqttSNClient::subscribersRetransmissions++;
}

void MqttSNSubscriber::retransmitSubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    TopicIdType topicIdType = lastSubscription.itemInfo->topicIdType;

    sendSubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, true, lastSubscription.itemInfo->qos,
                  topicIdType, std::stoi(msg->par("msgId").stringValue()), lastSubscription.topicName, lastSubscription.itemInfo->topicId,
                  (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));

    MqttSNClient::subscribersRetransmissions++;
}

void MqttSNSubscriber::retransmitUnsubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    TopicIdType topicIdType = lastUnsubscription.itemInfo->topicIdType;

    sendUnsubscribe(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, topicIdType,
                    std::stoi(msg->par("msgId").stringValue()), lastUnsubscription.topicName, lastUnsubscription.itemInfo->topicId,
                    (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID));

    MqttSNClient::subscribersRetransmissions++;
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
    cancelAndDelete(unsubscriptionEvent);
}

} /* namespace mqttsn */
