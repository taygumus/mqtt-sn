#include "MqttSNPublisher.h"
#include "externals/nlohmann/json.hpp"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNRegister.h"
#include "messages/MqttSNMsgIdWithTopicIdPlus.h"

namespace mqttsn {

Define_Module(MqttSNPublisher);

using json = nlohmann::json;

void MqttSNPublisher::initializeCustom()
{
    willQosFlag = par("willQosFlag");
    willRetainFlag = par("willRetainFlag");
    willTopic = par("willTopic").stringValue();
    willMsg = par("willMsg").stringValue();

    fillTopicsAndData();

    registrationInterval = par("registrationInterval");
    registrationEvent = new inet::ClockEvent("registrationTimer");

    publishInterval = par("publishInterval");
    publishEvent = new inet::ClockEvent("publishTimer");

    waitingInterval = par("waitingInterval");
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
    // TO DO
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
    sendBaseWithWillTopic(srcAddress, srcPort, MsgType::WILLTOPIC, MqttSNClient::intToQoS(willQosFlag), willRetainFlag, willTopic);
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

    if (!MqttSNClient::checkMsgIdForType(MsgType::REGISTER, payload->getMsgId())) {
        // this ACK message does not match the expected message ID
        return;
    }

    // stop retransmission mechanism; ACK with correct message ID is received
    MqttSNClient::unscheduleMsgRetransmission(MsgType::REGISTER);

    // now process and analyze message content as needed
    ReturnCode returnCode = payload->getReturnCode();

    if (returnCode == ReturnCode::REJECTED_CONGESTION) {
        lastRegistration.retry = true;
        scheduleClockEventAfter(waitingInterval, registrationEvent);
        return;
    }

    if (returnCode == ReturnCode::REJECTED_NOT_SUPPORTED) {
        lastRegistration.retry = false;
        scheduleClockEventAfter(waitingInterval, registrationEvent);
        return;
    }

    uint16_t topicId = payload->getTopicId();

    if (returnCode != ReturnCode::ACCEPTED || topicId == 0) {
        throw omnetpp::cRuntimeError("Unexpected error: Invalid return code or topic ID");
    }

    // handle operations when the registration is ACCEPTED with a valid topic ID
    lastRegistration.retry = false;
    topicIds[topicId] = lastRegistration.info;

    int* counter = &topicsAndData[lastRegistration.info.topicsAndDataKey].counter;

    // check if the counter has reached its maximum value
    if (*counter == std::numeric_limits<int>::max()) {
        *counter = 0;
    }
    else {
        (*counter)++;
    }

    scheduleClockEventAfter(registrationInterval, registrationEvent);
}

void MqttSNPublisher::sendBaseWithWillTopic(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic)
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

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendBaseWithWillMsg(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, std::string willMsg)
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

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendRegister(const inet::L3Address& destAddress, const int& destPort, uint16_t msgId, std::string topicName)
{
    const auto& payload = inet::makeShared<MqttSNRegister>();
    payload->setMsgType(MsgType::REGISTER);
    payload->setMsgId(msgId);
    payload->setTopicName(topicName);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("RegisterPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, par("willFlag"), par("cleanSessionFlag"), MqttSNClient::keepAlive);
}

void MqttSNPublisher::handleRegistrationEvent()
{
    std::string topicName;

    // if it's a retry, use the last sent element
    if (lastRegistration.retry) {
        topicName = lastRegistration.info.topicName;
    }
    else {
        // check for topics availability
        if (topicsAndData.empty()) {
            throw omnetpp::cRuntimeError("No topic available");
        }

        // randomly select an element from the map
        auto it = topicsAndData.begin();
        std::advance(it, intuniform(0, topicsAndData.size() - 1));

        topicName = MqttSNClient::concatenateStringWithCounter(it->second.topicName, it->second.counter);

        // update information about the last element
        lastRegistration.info.topicName = topicName;
        lastRegistration.info.topicsAndDataKey = it->first;
        lastRegistration.retry = true;
    }

    if (!MqttSNApp::setNextAvailableId(MqttSNClient::getUsedMsgIds(), MqttSNClient::currentMsgId)) {
        throw omnetpp::cRuntimeError("Failed to assign a new message ID. All available message IDs are in use");
    }

    sendRegister(MqttSNClient::selectedGateway.address, MqttSNClient::selectedGateway.port, MqttSNClient::currentMsgId, topicName);

    // schedule register retransmission
    std::map<std::string, std::string> parameters;
    parameters["msgId"] = std::to_string(MqttSNClient::currentMsgId);
    MqttSNClient::scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::REGISTER, &parameters);
}

void MqttSNPublisher::handlePublishEvent()
{
    // check for topics availability
    if (topicIds.empty()) {
        scheduleClockEventAfter(publishInterval, publishEvent);
        return;
    }

    // randomly select a topic from the map
    auto topicIterator = topicIds.begin();
    std::advance(topicIterator, intuniform(0, topicIds.size() - 1));

    std::map<int, DataInfo> data = topicsAndData[topicIterator->second.topicsAndDataKey].data;

    // check for data availability
    if (data.empty()) {
        scheduleClockEventAfter(publishInterval, publishEvent);
        return;
    }

    // randomly select a data from the map
    auto dataIterator = data.begin();
    std::advance(dataIterator, intuniform(0, data.size() - 1));

    ///
    int selectedTopicId = topicIterator->first;
    DataInfo selectedData = dataIterator->second;
    ///
}

void MqttSNPublisher::fillTopicsAndData()
{
    json jsonData = json::parse(par("topicsAndDataJson").stringValue());
    int topicsKey = 0;

    // iterate over json object keys (topics) and fill the data structures
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        TopicAndData topicAndData;
        topicAndData.topicName = it.key();

        int dataKey = 0;

        // iterate over json array elements (messages) and populate the structure
        for (const auto& messageData : it.value()) {
            DataInfo dataInfo;
            dataInfo.qosFlag = MqttSNClient::intToQoS(messageData["qos"]);
            dataInfo.retainFlag = messageData["retain"];
            dataInfo.message = messageData["message"];

            topicAndData.data[dataKey++] = dataInfo;
        }

        topicsAndData[topicsKey++] = topicAndData;
    }
}

void MqttSNPublisher::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg, MsgType msgType)
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

        default:
            break;
    }
}

void MqttSNPublisher::retransmitWillTopicUpd(const inet::L3Address& destAddress, const int& destPort)
{
    sendBaseWithWillTopic(destAddress, destPort, MsgType::WILLTOPICUPD, MqttSNClient::intToQoS(willQosFlag), willRetainFlag, willTopic);
}

void MqttSNPublisher::retransmitWillMsgUpd(const inet::L3Address& destAddress, const int& destPort)
{
    sendBaseWithWillMsg(destAddress, destPort, MsgType::WILLMSGUPD, willMsg);
}

void MqttSNPublisher::retransmitRegister(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    sendRegister(destAddress, destPort, std::stoi(msg->par("msgId").stringValue()), lastRegistration.info.topicName);
}

MqttSNPublisher::~MqttSNPublisher()
{
    cancelAndDelete(registrationEvent);
    cancelAndDelete(publishEvent);
}

} /* namespace mqttsn */
