#include "MqttSNApp.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/StringHelper.h"
#include "types/shared/Length.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNPingReq.h"
#include "messages/MqttSNDisconnect.h"

namespace mqttsn {

using json = nlohmann::json;

void MqttSNApp::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        levelOneInit();
    }
}

void MqttSNApp::socketDataArrived(inet::UdpSocket* socket, inet::Packet* packet)
{
    processPacket(packet);
}

void MqttSNApp::socketErrorArrived(inet::UdpSocket* socket, inet::Indication* indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << std::endl;
    delete indication;
}

void MqttSNApp::socketClosed(inet::UdpSocket* socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(-1);
}

void MqttSNApp::sendGwInfo(uint8_t gatewayId, const std::string& gatewayAddress, uint16_t gatewayPort)
{
    const auto& payload = inet::makeShared<MqttSNGwInfo>();
    payload->setMsgType(MsgType::GWINFO);
    payload->setGwId(gatewayId);
    payload->setGwAdd(gatewayAddress);
    payload->setGwPort(gatewayPort);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("GwInfoPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
}

void MqttSNApp::sendPingReq(const inet::L3Address& destAddress, const int& destPort, const std::string& clientId)
{
    const auto& payload = inet::makeShared<MqttSNPingReq>();
    payload->setMsgType(MsgType::PINGREQ);

    if (!clientId.empty()) {
        payload->setClientId(clientId);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("PingReqPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNApp::sendBase(const inet::L3Address& destAddress, const int& destPort, MsgType msgType)
{
    const auto& payload = inet::makeShared<MqttSNBase>();
    payload->setMsgType(msgType);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::WILLTOPICREQ:
            packetName = "WillTopicReqPacket";
            break;

        case MsgType::WILLMSGREQ:
            packetName = "WillMsgReqPacket";
            break;

        case MsgType::PINGRESP:
            packetName = "PingRespPacket";
            break;

        default:
            packetName = "BasePacket";
    }

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNApp::sendDisconnect(const inet::L3Address& destAddress, const int& destPort, uint16_t duration)
{
    const auto& payload = inet::makeShared<MqttSNDisconnect>();
    payload->setMsgType(MsgType::DISCONNECT);

    if (duration > 0) {
        payload->setDuration(duration);
    }

    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("DisconnectPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNApp::checkPacketIntegrity(const inet::B& receivedLength, const inet::B& fieldLength)
{
    if (receivedLength != fieldLength) {
        throw omnetpp::cRuntimeError(
                "Packet integrity error: Received length (%d bytes) does not match the expected length (%d bytes)",
                (uint16_t) receivedLength.get(),
                (uint16_t) fieldLength.get()
        );
    }
}

bool MqttSNApp::isSelfBroadcastAddress(const inet::L3Address& address)
{
    inet::L3Address selfBroadcastAddress = inet::L3Address("127.0.0.1");
    return (address == selfBroadcastAddress);
}

bool MqttSNApp::setNextAvailableId(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue)
{
    // ID=0 is considered invalid; ID=UINT16_MAX can be considered invalid
    uint16_t maxValue = allowMaxValue ? UINT16_MAX : UINT16_MAX - 1;

    // if the set is full, there is no available ID
    if (usedIds.size() >= maxValue) {
        return false;
    }

    // if the set is empty, increment the current ID
    if (usedIds.empty()) {
        currentId = (currentId >= maxValue) ? 1 : currentId + 1;
        return true;
    }

    // reset to one if the current ID is invalid (zero) or exceeds the maximum allowed value
    if (currentId == 0 || (!allowMaxValue && currentId > maxValue)) {
        currentId = 1;
    }

    // find the next available ID
    while (usedIds.find(currentId) != usedIds.end()) {
        currentId = (currentId >= maxValue) ? 1 : currentId + 1;
    }

    return true;
}

uint16_t MqttSNApp::getNewIdentifier(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue,
                                     const std::string& error)
{
    if (!setNextAvailableId(usedIds, currentId, allowMaxValue)) {
        throw omnetpp::cRuntimeError("%s", error.c_str());
    }

    return currentId;
}

bool MqttSNApp::isMinTopicLength(uint16_t topicLength)
{
    // validate whether the topic name meets the minimum required length
    if (topicLength < Length::TWO_OCTETS) {
        return false;
    }

    return true;
}

void MqttSNApp::checkTopicLength(uint16_t topicLength, TopicIdType topicIdType)
{
    if (!isMinTopicLength(topicLength)) {
        throw omnetpp::cRuntimeError("Topic name must be at least two characters long");
    }

    if (topicLength == Length::TWO_OCTETS && topicIdType != TopicIdType::SHORT_TOPIC_ID) {
        throw omnetpp::cRuntimeError("A two-character topic name must be identified as a short topic");
    }

    if (topicLength > Length::TWO_OCTETS && topicIdType == TopicIdType::SHORT_TOPIC_ID) {
        throw omnetpp::cRuntimeError("Short topic names must have exactly two characters");
    }
}

std::map<std::string, uint16_t> MqttSNApp::getPredefinedTopics()
{
    json jsonData = json::parse(par("predefinedTopicsJson").stringValue());

    std::set<std::string> topicNames;
    std::set<uint16_t> topicIds;

    std::map<std::string, uint16_t> result;

    // iterate over json array elements
    for (const auto& topic : jsonData) {
        // extract topic name
        std::string topicName = topic["name"];
        std::string encodedTopicName = StringHelper::base64Encode(topicName);

        // check for duplicate topic names
        if (topicNames.find(encodedTopicName) != topicNames.end()) {
            throw omnetpp::cRuntimeError("Duplicate topic name found: %s", topicName.c_str());
        }

        // extract predefined topic ID
        int topicId = topic["id"];

        // check for duplicate topic IDs
        if (topicIds.find(topicId) != topicIds.end()) {
            throw omnetpp::cRuntimeError("Duplicate topic ID found: %d", topicId);
        }

        // validate topic name length and type against specified criteria
        checkTopicLength(topicName.length(), TopicIdType::PRE_DEFINED_TOPIC_ID);

        // check if the topic ID is within the range
        if (topicId <= std::numeric_limits<uint16_t>::min() ||
            topicId >= std::numeric_limits<uint16_t>::max()) {

            throw omnetpp::cRuntimeError("Invalid predefined topic ID value");
        }

        // insert unique topic name and ID into respective sets
        topicNames.insert(encodedTopicName);
        topicIds.insert(topicId);

        // populate the result map with the encoded topic name and ID
        result[encodedTopicName] = topicId;
    }

    return result;
}

} /* namespace mqttsn */
