#include "MqttSNPublisher.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"
#include "helpers/StringHelper.h"
#include "helpers/PacketHelper.h"
#include "helpers/NumericHelper.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNMsgIdWithTopicIdPlus.h"
#include "messages/MqttSNBaseWithMsgId.h"

namespace mqttsn {

Define_Module(MqttSNPublisher);

using json = nlohmann::json;

void MqttSNPublisher::levelTwoInit()
{
    willQoS = par("willQoS");
    willRetain = par("willRetain");
    willTopic = par("willTopic").stringValue();
    willMsg = par("willMsg").stringValue();

    populateItems();

    registrationInterval = par("registrationInterval");
    registrationEvent = new inet::ClockEvent("registrationTimer");

    publishInterval = par("publishInterval");
    publishEvent = new inet::ClockEvent("publishTimer");
}

bool MqttSNPublisher::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    if (msg == registrationEvent) {
        handleRegistrationEvent();
    }
    else if (msg == publishEvent) {
        handlePublishEvent();
    }
    else {
        return false;
    }

    return true;
}

void MqttSNPublisher::scheduleActiveStateEventsCustom()
{
    resetAndPopulateTopics();
    lastPublish.topicId = 0;
}

void MqttSNPublisher::cancelActiveStateEventsCustom()
{
    cancelEvent(registrationEvent);
    cancelEvent(publishEvent);
}

void MqttSNPublisher::cancelActiveStateClockEventsCustom()
{
    cancelClockEvent(registrationEvent);
    cancelClockEvent(publishEvent);
}

void MqttSNPublisher::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    switch(msgType) {
        // packet types that are allowed only from the selected gateway
        case MsgType::WILLTOPICREQ:
        case MsgType::WILLMSGREQ:
            if (!MqttSNClient::isSelectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

        // packet types that are allowed only from the connected gateway
        case MsgType::WILLTOPICRESP:
        case MsgType::WILLMSGRESP:
        case MsgType::REGACK:
        case MsgType::PUBACK:
        case MsgType::PUBREC:
        case MsgType::PUBCOMP:
            if (!MqttSNClient::isConnectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

        default:
            break;
    }

    switch(msgType) {
        case MsgType::WILLTOPICREQ:
            processWillTopicReq(srcAddress, srcPort);
            break;

        case MsgType::WILLMSGREQ:
            processWillMsgReq(srcAddress, srcPort);
            break;

        case MsgType::WILLTOPICRESP:
            processWillResp(pk, true);
            break;

        case MsgType::WILLMSGRESP:
            processWillResp(pk, false);
            break;

        case MsgType::REGACK:
            processRegAck(pk);
            break;

        case MsgType::PUBACK:
            processPubAck(pk);
            break;

        case MsgType::PUBREC:
            processPubRec(pk, srcAddress, srcPort);
            break;

        case MsgType::PUBCOMP:
            processPubComp(pk);
            break;

        default:
            break;
    }
}

void MqttSNPublisher::processConnAckCustom()
{
    scheduleClockEventAfter(registrationInterval, registrationEvent);
    scheduleClockEventAfter(publishInterval, publishEvent);
}

void MqttSNPublisher::processWillTopicReq(const inet::L3Address& srcAddress, const int& srcPort)
{
    sendBaseWithWillTopic(srcAddress, srcPort, MsgType::WILLTOPIC, ConversionHelper::intToQoS(willQoS), willRetain, willTopic);
}

void MqttSNPublisher::processWillMsgReq(const inet::L3Address& srcAddress, const int& srcPort)
{
    sendBaseWithWillMsg(srcAddress, srcPort, MsgType::WILLMSG, willMsg);
}

void MqttSNPublisher::processWillResp(inet::Packet* pk, bool willTopic)
{
    const auto& payload = pk->peekData<MqttSNBaseWithReturnCode>();

    if (payload->getReturnCode() != ReturnCode::ACCEPTED) {
        return;
    }

    if (willTopic) {
        EV << "Will topic name updated" << std::endl;
        return;
    }

    EV << "Will message updated" << std::endl;
}

void MqttSNPublisher::processRegAck(inet::Packet* pk)
{
    const auto& payload = pk->peekData<MqttSNMsgIdWithTopicIdPlus>();

    // check if the ACK is correct; exit if not
    if (!MqttSNClient::processAckForMsgType(MsgType::REGISTER, payload->getMsgId())) {
        return;
    }

    // now process and analyze message content as needed
    ReturnCode returnCode = payload->getReturnCode();

    if (returnCode == ReturnCode::REJECTED_CONGESTION) {
        lastRegistration.retry = true;
        scheduleClockEventAfter(MqttSNClient::waitingInterval, registrationEvent);
        return;
    }

    if (returnCode == ReturnCode::REJECTED_NOT_SUPPORTED) {
        lastRegistration.retry = false;
        scheduleClockEventAfter(MqttSNClient::waitingInterval, registrationEvent);
        return;
    }

    uint16_t topicId = payload->getTopicId();

    if (returnCode != ReturnCode::ACCEPTED || topicId == 0) {
        throw omnetpp::cRuntimeError("Unexpected error: Invalid return code or topic ID");
    }

    // handle operations when the registration is ACCEPTED; update data structures
    TopicInfo& topicInfo = topics[topicId];
    topicInfo.topicName = lastRegistration.topicName;
    topicInfo.itemInfo = lastRegistration.itemInfo;

    NumericHelper::incrementCounter(&(lastRegistration.itemInfo->counter));

    lastRegistration.retry = false;
    scheduleClockEventAfter(registrationInterval, registrationEvent);

    registrationCounter++;
}

void MqttSNPublisher::processPubAck(inet::Packet* pk)
{
    const auto& payload = pk->peekData<MqttSNMsgIdWithTopicIdPlus>();

    // obtain the QoS level from the last published message
    QoS qos = lastPublish.dataInfo->qos;

    if (qos == QoS::QOS_ONE || qos == QoS::QOS_TWO) {
        // check if the ACK is correct; exit if not
        if (!MqttSNClient::processAckForMsgType(MsgType::PUBLISH, payload->getMsgId())) {
            return;
        }
    }

    // now process and analyze message content as needed
    ReturnCode returnCode = payload->getReturnCode();

    if (returnCode == ReturnCode::REJECTED_INVALID_TOPIC_ID) {
        // update registration information
        lastRegistration.topicName = lastPublish.topicName;
        lastRegistration.itemInfo = lastPublish.itemInfo;
        lastRegistration.retry = true;

        MqttSNClient::unscheduleMsgRetransmission(MsgType::REGISTER);
        cancelEvent(registrationEvent);

        // retry topic registration
        scheduleClockEventAfter(MqttSNClient::MIN_WAITING_TIME, registrationEvent);

        retryLastPublish();
        return;
    }

    if (returnCode == ReturnCode::REJECTED_CONGESTION) {
        retryLastPublish();
        return;
    }

    if (returnCode != ReturnCode::ACCEPTED) {
        throw omnetpp::cRuntimeError("Unexpected error: Invalid return code");
    }

    // handle operations when publish is ACCEPTED
    lastPublish.retry = false;
    scheduleClockEventAfter(publishInterval, publishEvent);

    publishCounter++;
}

void MqttSNPublisher::processPubRec(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithMsgId>();
    uint16_t msgId = payload->getMsgId();

    // check if the ACK is correct; exit if not
    if (!MqttSNClient::processAckForMsgType(MsgType::PUBLISH, msgId)) {
        return;
    }

    // send publish release
    sendBaseWithMsgId(srcAddress, srcPort, MsgType::PUBREL, msgId);

    // schedule publish release retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::PUBREL, msgId);
}

void MqttSNPublisher::processPubComp(inet::Packet* pk)
{
    const auto& payload = pk->peekData<MqttSNBaseWithMsgId>();

    // check if the ACK is correct; exit if not
    if (!MqttSNClient::processAckForMsgType(MsgType::PUBREL, payload->getMsgId())) {
        return;
    }

    // proceed with the next publish
    lastPublish.retry = false;
    scheduleClockEventAfter(publishInterval, publishEvent);

    publishCounter++;
}

void MqttSNPublisher::sendBaseWithWillTopic(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, QoS qosFlag,
                                            bool retainFlag, const std::string& willTopic)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillTopic>();
    payload->setMsgType(msgType);
    payload->setQoSFlag(qosFlag);
    payload->setRetainFlag(retainFlag);
    payload->setWillTopic(willTopic);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::WILLTOPIC:
            packetName = "WillTopicPacket";
            break;

        case MsgType::WILLTOPICUPD:
            packetName = "WillTopicUpdPacket";
            break;

        default:
            packetName = "BaseWithWillTopicPacket";
    }

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendBaseWithWillMsg(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, const std::string& willMsg)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillMsg>();
    payload->setMsgType(msgType);
    payload->setWillMsg(willMsg);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::WILLMSG:
            packetName = "WillMsgPacket";
            break;

        case MsgType::WILLMSGUPD:
            packetName = "WillMsgUpdPacket";
            break;

        default:
            packetName = "BaseWithWillMsgPacket";
    }

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendRegister(const inet::L3Address& destAddress, const int& destPort, uint16_t msgId, const std::string& topicName)
{
    MqttSNApp::socket.sendTo(PacketHelper::getRegisterPacket(0, msgId, topicName), destAddress, destPort);
}

void MqttSNPublisher::sendPublish(const inet::L3Address& destAddress, const int& destPort, bool dupFlag, QoS qosFlag, bool retainFlag,
                                  TopicIdType topicIdTypeFlag, uint16_t topicId, uint16_t msgId, const std::string& data)
{
    MqttSNApp::socket.sendTo(
            PacketHelper::getPublishPacket(dupFlag, qosFlag, retainFlag, topicIdTypeFlag, topicId, msgId, data),
            destAddress,
            destPort
    );
}

void MqttSNPublisher::sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId)
{
    MqttSNApp::socket.sendTo(PacketHelper::getBaseWithMsgIdPacket(msgType, msgId), destAddress, destPort);
}

void MqttSNPublisher::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, par("will"), par("cleanSession"), MqttSNClient::keepAlive);
}

void MqttSNPublisher::handleRegistrationEvent()
{
    if (!proceedWithRegistration()) {
        return;
    }

    sendRegister(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, MqttSNClient::getNewMsgId(),
                 lastRegistration.topicName);

    // schedule register retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::REGISTER, MqttSNClient::currentMsgId);
}

void MqttSNPublisher::handlePublishEvent()
{
    if (!proceedWithPublish()) {
        return;
    }

    // print publish message to be sent
    printPublishMessage();

    QoS qos = lastPublish.dataInfo->qos;

    if (qos == QoS::QOS_ZERO) {
        sendPublish(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, false, qos, lastPublish.dataInfo->retain,
                    lastPublish.itemInfo->topicIdType, lastPublish.topicId, 0, lastPublish.dataInfo->data);

        // no need to wait for an ACK
        scheduleClockEventAfter(publishInterval, publishEvent);
        return;
    }

    sendPublish(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, false, qos, lastPublish.dataInfo->retain,
                lastPublish.itemInfo->topicIdType, lastPublish.topicId, MqttSNClient::getNewMsgId(), lastPublish.dataInfo->data);

    // schedule publish retransmission
    MqttSNClient::scheduleRetransmissionWithMsgId(MsgType::PUBLISH, MqttSNClient::currentMsgId);
}

void MqttSNPublisher::populateItems()
{
    json jsonData = json::parse(par("itemsJson").stringValue());
    int itemsKey = 0;

    // iterate over json array elements
    for (const auto& item : jsonData) {
        std::string topicName = item["topic"];
        TopicIdType topicIdType = ConversionHelper::stringToTopicIdType(item["idType"]);

        // validate topic name length and type against specified criteria
        MqttSNApp::checkTopicLength(topicName.length(), topicIdType);

        // validate topic consistency
        MqttSNClient::checkTopicConsistency(
                topicName, topicIdType,
                MqttSNClient::predefinedTopics.find(StringHelper::base64Encode(topicName)) != MqttSNClient::predefinedTopics.end()
        );

        ItemInfo itemInfo;
        itemInfo.topicName = topicName;
        itemInfo.topicIdType = topicIdType;

        int dataKey = 0;

        // iterate over data array within the current item
        for (const auto& data : item["data"]) {
            DataInfo dataInfo;
            dataInfo.qos = ConversionHelper::intToQoS(data["qos"]);
            dataInfo.retain = data["retain"];
            dataInfo.data = data["data"];

            itemInfo.data[dataKey++] = dataInfo;
        }

        items[itemsKey++] = itemInfo;
    }
}

void MqttSNPublisher::resetAndPopulateTopics()
{
    topics.clear();

    for (auto& pair : items) {
        ItemInfo& itemInfo = pair.second;

        // reset the counter if the topic uses a short ID type
        if (itemInfo.topicIdType == TopicIdType::SHORT_TOPIC_ID) {
            itemInfo.counter = 0;
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

bool MqttSNPublisher::findTopicByName(const std::string& topicName, uint16_t& topicId)
{
    // iterate through the map to find the specified topic name
    for (const auto& pair : topics) {
        if (pair.second.topicName == topicName) {
            // topic name found in the map, assign the corresponding topic ID and return true
            topicId = pair.first;
            return true;
        }
    }

    // topic name not found in the map
    return false;
}

bool MqttSNPublisher::proceedWithRegistration()
{
    // check if a registration attempt needs to be retried
    if (lastRegistration.retry) {
        return true;
    }

    int registrationLimit = par("registrationLimit");
    // check if the registration limit is set and reached
    if (registrationLimit != -1 && registrationLimit == registrationCounter) {
        return false;
    }

    // check for items availability
    if (items.empty()) {
        throw omnetpp::cRuntimeError("No item available");
    }

    // randomly select an element from the map
    auto it = items.begin();
    std::advance(it, intuniform(0, items.size() - 1));

    TopicIdType topicIdType = it->second.topicIdType;
    int counter = it->second.counter;

    // short topics are registered only once; predefined topics are ignored for registration
    if ((topicIdType == TopicIdType::SHORT_TOPIC_ID && counter == 1) ||
        (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID)) {

        scheduleClockEventAfter(MqttSNClient::MIN_WAITING_TIME, registrationEvent);
        return false;
    }

    // update information about the last element
    lastRegistration.topicName = StringHelper::appendCounterToString(it->second.topicName, MqttSNClient::TOPIC_DELIMITER, counter);
    lastRegistration.itemInfo = &it->second;
    lastRegistration.retry = true;

    return true;
}

void MqttSNPublisher::printPublishMessage()
{
    EV << "Publish message to be sent:" << std::endl;
    EV << "Topic name: " << lastPublish.topicName << std::endl;
    EV << "Topic ID: " << lastPublish.topicId << std::endl;
    EV << "Topic ID type: " << ConversionHelper::topicIdTypeToString(lastPublish.itemInfo->topicIdType) << std::endl;
    EV << "Duplicate: " << false << std::endl;
    EV << "QoS: " << ConversionHelper::qosToInt(lastPublish.dataInfo->qos) << std::endl;
    EV << "Retain: " << lastPublish.dataInfo->retain << std::endl;
    EV << "Data: " << lastPublish.dataInfo->data << std::endl;
}

void MqttSNPublisher::retryLastPublish()
{
    lastPublish.retry = true;

    cancelEvent(publishEvent);
    scheduleClockEventAfter(MqttSNClient::waitingInterval, publishEvent);
}

bool MqttSNPublisher::proceedWithPublish()
{
    // if it's a retry, use the last sent element
    if (lastPublish.retry) {
        // update topic ID after publisher reconnection to a server
        if (lastPublish.topicId == 0) {
            findTopicByName(lastPublish.topicName, lastPublish.topicId);
        }
        return true;
    }

    int publishLimit = par("publishLimit");
    // check if the publish limit is set and reached
    if (publishLimit != -1 && publishLimit == publishCounter) {
        return false;
    }

    // check for topics availability
    if (topics.empty()) {
        scheduleClockEventAfter(publishInterval, publishEvent);
        return false;
    }

    // randomly select a topic from the map
    auto topicIterator = topics.begin();
    std::advance(topicIterator, intuniform(0, topics.size() - 1));

    const TopicInfo& topicInfo = topicIterator->second;

    // retrieve topic data
    std::map<int, DataInfo>& data = topicInfo.itemInfo->data;

    // check for data availability
    if (data.empty()) {
        scheduleClockEventAfter(publishInterval, publishEvent);
        return false;
    }

    // randomly select a data from the map
    auto dataIterator = data.begin();
    std::advance(dataIterator, intuniform(0, data.size() - 1));

    // update information about the last element
    lastPublish.topicName = topicInfo.topicName;
    lastPublish.topicId = topicIterator->first;
    lastPublish.itemInfo = topicInfo.itemInfo;
    lastPublish.dataInfo = &dataIterator->second;

    // retry after publisher reconnection to a server
    if (dataIterator->second.qos != QoS::QOS_ZERO) {
        lastPublish.retry = true;
    }

    return true;
}

void MqttSNPublisher::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg,
                                                      MsgType msgType)
{
    switch (msgType) {
        case MsgType::WILLTOPICUPD:
            retransmitWillTopicUpd(destAddress, destPort);
            break;

        case MsgType::WILLMSGUPD:
            retransmitWillMsgUpd(destAddress, destPort);
            break;

        case MsgType::REGISTER:
            retransmitRegister(destAddress, destPort, msg);
            break;

        case MsgType::PUBLISH:
            retransmitPublish(destAddress, destPort, msg);
            break;

        case MsgType::PUBREL:
            retransmitPubRel(destAddress, destPort, msg);
            break;

        default:
            break;
    }
}

void MqttSNPublisher::retransmitWillTopicUpd(const inet::L3Address& destAddress, const int& destPort)
{
    sendBaseWithWillTopic(destAddress, destPort, MsgType::WILLTOPICUPD, ConversionHelper::intToQoS(willQoS), willRetain, willTopic);
}

void MqttSNPublisher::retransmitWillMsgUpd(const inet::L3Address& destAddress, const int& destPort)
{
    sendBaseWithWillMsg(destAddress, destPort, MsgType::WILLMSGUPD, willMsg);
}

void MqttSNPublisher::retransmitRegister(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendRegister(destAddress, destPort, std::stoi(msg->par("msgId").stringValue()), lastRegistration.topicName);
}

void MqttSNPublisher::retransmitPublish(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendPublish(destAddress, destPort, true, lastPublish.dataInfo->qos, lastPublish.dataInfo->retain, lastPublish.itemInfo->topicIdType,
                lastPublish.topicId, std::stoi(msg->par("msgId").stringValue()), lastPublish.dataInfo->data);
}

void MqttSNPublisher::retransmitPubRel(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendBaseWithMsgId(destAddress, destPort, MsgType::PUBREL, std::stoi(msg->par("msgId").stringValue()));
}

MqttSNPublisher::~MqttSNPublisher()
{
    cancelAndDelete(registrationEvent);
    cancelAndDelete(publishEvent);
}

} /* namespace mqttsn */
