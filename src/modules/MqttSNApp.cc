#include "MqttSNApp.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNPingReq.h"
#include "messages/MqttSNDisconnect.h"

namespace mqttsn {

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

void MqttSNApp::sendGwInfo(uint8_t gatewayId, std::string gatewayAddress, uint16_t gatewayPort)
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

void MqttSNApp::sendPingReq(const inet::L3Address& destAddress, const int& destPort, std::string clientId)
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
    uint16_t maxValue = UINT16_MAX - 1;
    if (allowMaxValue) {
        maxValue++;
    }

    // if the set is full, there is no available ID
    if (usedIds.size() >= maxValue) {
        return false;
    }

    // if the set is empty, increment the current ID
    if (usedIds.empty()) {
        currentId = (currentId >= maxValue) ? 1 : currentId + 1;
        return true;
    }

    // reset to one if the current ID is invalid (zero)
    if (currentId == 0) {
        currentId = 1;
    }

    // find the next available ID
    while (usedIds.find(currentId) != usedIds.end()) {
        currentId = (currentId >= maxValue) ? 1 : currentId + 1;
    }

    return true;
}

std::string MqttSNApp::sanitizeSpaces(std::string inputString)
{
    std::string sanitizedString = inputString;
    sanitizedString.erase(std::remove_if(sanitizedString.begin(), sanitizedString.end(), ::isspace), sanitizedString.end());

    return sanitizedString;
}

} /* namespace mqttsn */
