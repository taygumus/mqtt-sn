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

void MqttSNApp::sendPingReq(inet::L3Address destAddress, int destPort, std::string clientId)
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

void MqttSNApp::sendBase(inet::L3Address destAddress, int destPort, MsgType msgType)
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

void MqttSNApp::sendDisconnect(inet::L3Address destAddress, int destPort, uint16_t duration)
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

void MqttSNApp::checkPacketIntegrity(inet::B receivedLength, inet::B fieldLength)
{
    if (receivedLength != fieldLength) {
        throw omnetpp::cRuntimeError(
                "Packet integrity error: Received length (%d bytes) does not match the expected length (%d bytes)",
                (uint16_t) receivedLength.get(),
                (uint16_t) fieldLength.get()
        );
    }
}

bool MqttSNApp::isSelfBroadcastAddress(inet::L3Address address)
{
    inet::L3Address selfBroadcastAddress = inet::L3Address("127.0.0.1");
    return (address == selfBroadcastAddress);
}

} /* namespace mqttsn */
