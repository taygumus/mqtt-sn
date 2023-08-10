#include "MqttSNServer.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNConnect.h"
#include "messages/MqttSNBase.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"

namespace mqttsn {

Define_Module(MqttSNServer);

int MqttSNServer::gatewayIdCounter = -1;

void MqttSNServer::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        startAdvertise = par("startAdvertise");
        stopAdvertise = par("stopAdvertise");
        advertiseInterval = par("advertiseInterval");

        if (stopAdvertise >= inet::CLOCKTIME_ZERO && stopAdvertise < startAdvertise) {
            throw omnetpp::cRuntimeError("Invalid startAdvertise/stopAdvertise parameters");
        }
        advertiseEvent = new inet::ClockEvent("advertiseTimer");

        if (gatewayIdCounter < UINT8_MAX) {
            gatewayIdCounter++;
        }
        else {
            throw omnetpp::cRuntimeError("The gateway ID counter has reached its maximum limit");
        }
        gatewayId = gatewayIdCounter;
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == advertiseEvent) {
        handleAdvertiseEvent();
    }
    else {
        socket.processMessage(msg);
    }
}

void MqttSNServer::finish()
{
    inet::ApplicationBase::finish();
}

void MqttSNServer::refreshDisplay() const
{
    inet::ApplicationBase::refreshDisplay();
}

void MqttSNServer::handleStartOperation(inet::LifecycleOperation *operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    inet::clocktime_t start = std::max(startAdvertise, getClockTime());
    if ((stopAdvertise < inet::CLOCKTIME_ZERO) || (start < stopAdvertise) || (start == stopAdvertise && startAdvertise == stopAdvertise)) {
        scheduleClockEventAt(start, advertiseEvent);
    }
    else {
        activeGateway = false;
    }
}

void MqttSNServer::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(advertiseEvent);
    socket.close();
}

void MqttSNServer::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(advertiseEvent);
    socket.destroy();
}

void MqttSNServer::processPacket(inet::Packet *pk)
{
    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();

    if (!activeGateway || isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

    EV << "Server received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    const auto& header = pk->peekData<MqttSNBase>();
    checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    // packet types that require a connection session
    switch(header->getMsgType()) {
        case MsgType::WILLTOPIC:
        case MsgType::WILLMSG:
            if (!isClientConnected(srcAddress, srcPort)) {
                delete pk;
                return;
            }
            break;

        default:
            break;
    }

    switch(header->getMsgType()) {
        case MsgType::SEARCHGW:
            processSearchGw(pk);
            break;

        case MsgType::CONNECT:
            processConnect(pk, srcAddress, srcPort);
            break;

        case MsgType::WILLTOPIC:
            processWillTopic(pk, srcAddress, srcPort);
            break;

        case MsgType::WILLMSG:
            processWillMsg(pk, srcAddress, srcPort);
            break;

        default:
            break;
    }

    delete pk;
}

void MqttSNServer::processSearchGw(inet::Packet *pk)
{
    MqttSNApp::sendGwInfo(gatewayId);
}

void MqttSNServer::processConnect(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNConnect>();

    if (payload->getProtocolId() != 0x01) {
        return;
    }

    updateConnectedClients(srcAddress, srcPort, payload->getClientId());

    // TO DO -> keep alive

    if (payload->getWillFlag()) {
        sendBase(MsgType::WILLTOPICREQ, srcAddress, srcPort);
    }
    else {
        // TO DO -> return code
        sendBaseWithReturnCode(MsgType::CONNACK, ReturnCode::ACCEPTED, srcAddress, srcPort);
    }
}

void MqttSNServer::processWillTopic(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillTopic>();

    // TO DO -> save information about willFlag, willTopic, qosFlag, retainFlag
    // TO DO -> QOS, retain flags management

    sendBase(MsgType::WILLMSGREQ, srcAddress, srcPort);
}

void MqttSNServer::processWillMsg(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillMsg>();

    // TO DO -> save information about will message

    // TO DO -> return code
    sendBaseWithReturnCode(MsgType::CONNACK, ReturnCode::ACCEPTED, srcAddress, srcPort);
}

void MqttSNServer::sendAdvertise()
{
    const auto& payload = inet::makeShared<MqttSNAdvertise>();
    payload->setMsgType(MsgType::ADVERTISE);
    payload->setGwId(gatewayId);
    payload->setDuration(advertiseInterval);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::ostringstream str;
    str << "AdvertisePacket"<< "-" << numAdvertiseSent;
    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
    numAdvertiseSent++;
}

void MqttSNServer::sendBase(MsgType msgType, inet::L3Address destAddress, int destPort)
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

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNServer::sendBaseWithReturnCode(MsgType msgType, ReturnCode returnCode, inet::L3Address destAddress, int destPort)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithReturnCode>();
    payload->setMsgType(msgType);
    payload->setReturnCode(returnCode);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::CONNACK:
            packetName = "ConnAckPacket";
            break;

        case MsgType::WILLTOPICRESP:
            packetName = "WillTopicRespPacket";
            break;

        case MsgType::WILLMSGRESP:
            packetName = "WillMsgRespPacket";
            break;

        default:
            packetName = "BaseWithReturnCodePacket";
    }

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNServer::handleAdvertiseEvent()
{
    if (!lastAdvertise) {
        sendAdvertise();
    }

    if ((stopAdvertise < inet::CLOCKTIME_ZERO) || (getClockTime() + advertiseInterval < stopAdvertise)) {
        scheduleClockEventAfter(advertiseInterval, advertiseEvent);
    }
    else {
        inet::clocktime_t remainingTime = stopAdvertise - getClockTime();
        // keep gateway active for the remaining time but without sending an advertise message
        if (remainingTime > inet::CLOCKTIME_ZERO) {
            scheduleClockEventAfter(remainingTime, advertiseEvent);
            lastAdvertise = true;
        }
        else {
            activeGateway = false;
        }
    }
}

void MqttSNServer::updateConnectedClients(inet::L3Address srcAddress, int srcPort, std::string clientId)
{
    auto key = std::make_pair(srcAddress, srcPort);
    auto it = connectedClients.find(key);

    if (it == connectedClients.end()) {
        ClientInfo clientInfo;
        clientInfo.clientId = clientId;

        connectedClients[key] = clientInfo;
    }
    else {
        // TO DO
    }
}

bool MqttSNServer::isClientConnected(inet::L3Address srcAddress, int srcPort)
{
    // check if the client with the specified address and port is present in the data structure
    return (connectedClients.find(std::make_pair(srcAddress, srcPort)) != connectedClients.end());
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseEvent);
}

} /* namespace mqttsn */
