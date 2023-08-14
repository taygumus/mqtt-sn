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

    bool willFlag = payload->getWillFlag();

    // prepare client information
    ClientInfo clientInfo;
    clientInfo.clientId = payload->getClientId();
    clientInfo.willFlag = willFlag;

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.clientId = true;
    updates.willFlag = true;

    // update or save client information
    updateConnectedClients(srcAddress, srcPort, clientInfo, updates);

    // TO DO -> keep alive

    if (willFlag) {
        sendBase(MsgType::WILLTOPICREQ, srcAddress, srcPort);
    }
    else {
        // TO DO -> return code
        sendBaseWithReturnCode(MsgType::CONNACK, ReturnCode::ACCEPTED, srcAddress, srcPort);
    }
}

void MqttSNServer::processWillTopic(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithWillTopic>();

    // prepare client information
    ClientInfo clientInfo;
    clientInfo.qosFlag = (QoS) payload->getQoSFlag();
    clientInfo.retainFlag = payload->getRetainFlag();
    clientInfo.willTopic = payload->getWillTopic();

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.qosFlag = true;
    updates.retainFlag = true;
    updates.willTopic = true;

    // update or save client information
    updateConnectedClients(srcAddress, srcPort, clientInfo, updates);

    // TO DO -> QOS, retain flags management

    sendBase(MsgType::WILLMSGREQ, srcAddress, srcPort);
}

void MqttSNServer::processWillMsg(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithWillMsg>();

    // prepare client information
    ClientInfo clientInfo;
    clientInfo.willMsg = payload->getWillMsg();

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.willMsg = true;

    // update or save client information
    updateConnectedClients(srcAddress, srcPort, clientInfo, updates);

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

void MqttSNServer::updateConnectedClients(inet::L3Address srcAddress, int srcPort, ClientInfo& clientInfo, ClientInfoUpdates& updates)
{
    auto key = std::make_pair(srcAddress, srcPort);
    auto it = connectedClients.find(key);

    if (it == connectedClients.end()) {
        ClientInfo newClientInfo;
        applyClientUpdates(newClientInfo, clientInfo, updates);

        connectedClients[key] = newClientInfo;
    }
    else {
        ClientInfo& existingClientInfo = it->second;
        applyClientUpdates(existingClientInfo, clientInfo, updates);
    }
}

void MqttSNServer::applyClientUpdates(ClientInfo& existingClientInfo, ClientInfo& newClientInfo, ClientInfoUpdates& updates)
{
    if (updates.clientId) {
        existingClientInfo.clientId = newClientInfo.clientId;
    }
    if (updates.dupFlag) {
        existingClientInfo.dupFlag = newClientInfo.dupFlag;
    }
    if (updates.qosFlag) {
        existingClientInfo.qosFlag = newClientInfo.qosFlag;
    }
    if (updates.retainFlag) {
        existingClientInfo.retainFlag = newClientInfo.retainFlag;
    }
    if (updates.willFlag) {
        existingClientInfo.willFlag = newClientInfo.willFlag;
    }
    if (updates.cleanSessionFlag) {
        existingClientInfo.cleanSessionFlag = newClientInfo.cleanSessionFlag;
    }
    if (updates.topicIdTypeFlag) {
        existingClientInfo.topicIdTypeFlag = newClientInfo.topicIdTypeFlag;
    }
    if (updates.willTopic) {
        existingClientInfo.willTopic = newClientInfo.willTopic;
    }
    if (updates.willMsg) {
        existingClientInfo.willMsg = newClientInfo.willMsg;
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
