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
#include "messages/MqttSNDisconnect.h"

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

        activeClientsCheckInterval = par("activeClientsCheckInterval");
        activeClientsCheckEvent = new inet::ClockEvent("activeClientsCheckTimer");

        asleepClientsCheckInterval = par("asleepClientsCheckInterval");
        asleepClientsCheckEvent = new inet::ClockEvent("asleepClientsCheckTimer");
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == advertiseEvent) {
        handleAdvertiseEvent();
    }
    else if (msg == activeClientsCheckEvent) {
        handleActiveClientsCheckEvent();
    }
    else if (msg == asleepClientsCheckEvent) {
        handleAsleepClientsCheckEvent();
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

    scheduleClockEventAt(activeClientsCheckInterval, activeClientsCheckEvent);
    scheduleClockEventAt(asleepClientsCheckInterval, asleepClientsCheckEvent);
}

void MqttSNServer::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(advertiseEvent);
    cancelEvent(activeClientsCheckEvent);
    cancelEvent(asleepClientsCheckEvent);

    socket.close();
}

void MqttSNServer::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(advertiseEvent);
    cancelClockEvent(activeClientsCheckEvent);
    cancelClockEvent(asleepClientsCheckEvent);

    socket.destroy();
}

void MqttSNServer::processPacket(inet::Packet *pk)
{
    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();

    if (!activeGateway || MqttSNApp::isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

    EV << "Server received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    const auto& header = pk->peekData<MqttSNBase>();
    MqttSNApp::checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    // packet types that require an ACTIVE client state
    switch(header->getMsgType()) {
        case MsgType::WILLTOPIC:
        case MsgType::WILLMSG:
        case MsgType::PINGREQ:
        case MsgType::PINGRESP:
        case MsgType::DISCONNECT:
            if (!isClientInState(srcAddress, srcPort, ClientState::ACTIVE)) {
                delete pk;
                return;
            }
            break;

        default:
            break;
    }

    switch(header->getMsgType()) {
        case MsgType::SEARCHGW:
            processSearchGw();
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

        case MsgType::PINGREQ:
            processPingReq(srcAddress, srcPort);
            break;

        case MsgType::PINGRESP:
            processPingResp(srcAddress, srcPort);
            break;

        case MsgType::DISCONNECT:
            processDisconnect(pk, srcAddress, srcPort);
            break;

        default:
            break;
    }

    delete pk;
}

void MqttSNServer::processSearchGw()
{
    MqttSNApp::sendGwInfo(gatewayId);
}

void MqttSNServer::processConnect(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNConnect>();

    // prevent client connection when its protocol ID is not supported
    if (payload->getProtocolId() != 0x01) {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::REJECTED_NOT_SUPPORTED);
        return;
    }

    // prevent new client connections when the gateway is congested
    if (isGatewayCongested() && !isClientExists(srcAddress, srcPort)) {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::REJECTED_CONGESTION);
        return;
    }

    bool willFlag = payload->getWillFlag();

    // prepare client information
    ClientInfo clientInfo;
    clientInfo.clientId = payload->getClientId();
    clientInfo.keepAliveDuration = payload->getDuration();
    clientInfo.willFlag = willFlag;
    clientInfo.cleanSessionFlag = payload->getCleanSessionFlag();
    clientInfo.currentState = ClientState::ACTIVE;
    clientInfo.lastReceivedMsgTime = getClockTime();

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.clientId = true;
    updates.keepAliveDuration = true;
    updates.willFlag = true;
    updates.cleanSessionFlag = true;
    updates.currentState = true;
    updates.lastReceivedMsgTime = true;

    // update or save client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);

    if (willFlag) {
        MqttSNApp::sendBase(srcAddress, srcPort, MsgType::WILLTOPICREQ);
    }
    else {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::ACCEPTED);
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
    clientInfo.lastReceivedMsgTime = getClockTime();

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.qosFlag = true;
    updates.retainFlag = true;
    updates.willTopic = true;
    updates.lastReceivedMsgTime = true;

    // update client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);

    MqttSNApp::sendBase(srcAddress, srcPort, MsgType::WILLMSGREQ);
}

void MqttSNServer::processWillMsg(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithWillMsg>();

    // prepare client information
    ClientInfo clientInfo;
    clientInfo.willMsg = payload->getWillMsg();
    clientInfo.lastReceivedMsgTime = getClockTime();

    // specify which fields to update
    ClientInfoUpdates updates;
    updates.willMsg = true;
    updates.lastReceivedMsgTime = true;

    // update client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);

    sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::ACCEPTED);
}

void MqttSNServer::processPingReq(inet::L3Address srcAddress, int srcPort)
{
    ClientInfo clientInfo;
    clientInfo.lastReceivedMsgTime = getClockTime();

    ClientInfoUpdates updates;
    updates.lastReceivedMsgTime = true;

    // update client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);

    MqttSNApp::sendBase(srcAddress, srcPort, MsgType::PINGRESP);
}

void MqttSNServer::processPingResp(inet::L3Address srcAddress, int srcPort)
{
    EV << "Received ping response from client: " << srcAddress << ":" << srcPort << std::endl;

    ClientInfo clientInfo;
    clientInfo.lastReceivedMsgTime = getClockTime();
    clientInfo.sentPingReq = false;

    ClientInfoUpdates updates;
    updates.lastReceivedMsgTime = true;
    updates.sentPingReq = true;

    // update client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);
}

void MqttSNServer::processDisconnect(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNDisconnect>();
    uint16_t sleepDuration = payload->getDuration();

    ClientInfo clientInfo;
    clientInfo.sleepDuration = sleepDuration;

    if (sleepDuration > 0) {
        clientInfo.currentState = ClientState::ASLEEP;
    }
    else {
        clientInfo.currentState = ClientState::DISCONNECTED;
    }

    clientInfo.lastReceivedMsgTime = getClockTime();

    ClientInfoUpdates updates;
    updates.sleepDuration = true;
    updates.currentState = true;
    updates.lastReceivedMsgTime = true;

    // update client information
    updateClientInfo(srcAddress, srcPort, clientInfo, updates);

    // ack with disconnect message
    MqttSNApp::sendDisconnect(srcAddress, srcPort, sleepDuration);

    // TO DO -> not affect existing subscriptions (6.12)
    // TO DO -> manage disconnect with sleep duration field (6.14)
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

void MqttSNServer::sendBaseWithReturnCode(inet::L3Address destAddress, int destPort, MsgType msgType, ReturnCode returnCode)
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

void MqttSNServer::handleActiveClientsCheckEvent()
{
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        ClientInfo& clientInfo = it->second;

        // check if the client is ACTIVE and if the elapsed time from last received message is beyond the keep alive duration
        if (clientInfo.currentState == ClientState::ACTIVE &&
            (currentTime - clientInfo.lastReceivedMsgTime) > clientInfo.keepAliveDuration) {

            if (clientInfo.sentPingReq) {
                // change the expired client state and activate the Will feature
                clientInfo.currentState = ClientState::LOST;
                // TO DO -> Will feature activation
            }
            else {
                // send a solicitation ping request to the expired client
                const std::pair<inet::L3Address, int> clientKey = it->first;

                MqttSNApp::sendPingReq(clientKey.first, clientKey.second);
                clientInfo.sentPingReq = true;
            }
        }
    }

    scheduleClockEventAfter(activeClientsCheckInterval, activeClientsCheckEvent);
}

void MqttSNServer::handleAsleepClientsCheckEvent()
{
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        ClientInfo& clientInfo = it->second;

        // check if the client is ASLEEP and if the elapsed time from last received message is beyond the sleep duration
        if (clientInfo.currentState == ClientState::ASLEEP &&
            (currentTime - clientInfo.lastReceivedMsgTime) > clientInfo.sleepDuration) {

            // change the expired client state and activate the Will feature
            clientInfo.currentState = ClientState::LOST;
            // TO DO -> Will feature activation
            // TO DO -> Buffering of messages to send
        }
    }

    scheduleClockEventAfter(asleepClientsCheckInterval, asleepClientsCheckEvent);
}

void MqttSNServer::updateClientInfo(inet::L3Address srcAddress, int srcPort, ClientInfo& clientInfo, ClientInfoUpdates& updates)
{
    auto key = std::make_pair(srcAddress, srcPort);
    auto it = clients.find(key);

    if (it == clients.end()) {
        ClientInfo newClientInfo;
        applyClientInfoUpdates(newClientInfo, clientInfo, updates);

        clients[key] = newClientInfo;
    }
    else {
        ClientInfo& existingClientInfo = it->second;
        applyClientInfoUpdates(existingClientInfo, clientInfo, updates);
    }
}

void MqttSNServer::applyClientInfoUpdates(ClientInfo& existingClientInfo, ClientInfo& newClientInfo, ClientInfoUpdates& updates)
{
    if (updates.clientId) {
        existingClientInfo.clientId = newClientInfo.clientId;
    }
    if (updates.willTopic) {
        existingClientInfo.willTopic = newClientInfo.willTopic;
    }
    if (updates.willMsg) {
        existingClientInfo.willMsg = newClientInfo.willMsg;
    }
    if (updates.keepAliveDuration) {
        existingClientInfo.keepAliveDuration = newClientInfo.keepAliveDuration;
    }
    if (updates.sleepDuration) {
        existingClientInfo.sleepDuration = newClientInfo.sleepDuration;
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
    if (updates.currentState) {
        existingClientInfo.currentState = newClientInfo.currentState;
    }
    if (updates.lastReceivedMsgTime) {
        existingClientInfo.lastReceivedMsgTime = newClientInfo.lastReceivedMsgTime;
    }
    if (updates.sentPingReq) {
        existingClientInfo.sentPingReq = newClientInfo.sentPingReq;
    }
}

bool MqttSNServer::isGatewayCongested()
{
    // check for gateway congestion based on clients count
    return clients.size() >= (unsigned int) par("maximumClients");
}

bool MqttSNServer::isClientExists(inet::L3Address srcAddress, int srcPort, ClientInfo *clientInfo)
{
    // check if the client with the specified address and port is present in the data structure
    auto clientIterator = clients.find(std::make_pair(srcAddress, srcPort));

    if (clientIterator != clients.end()) {
        // if the client exists, update the reference and return true
        if (clientInfo != nullptr) {
            *clientInfo = clientIterator->second;
        }

        return true;
    }
    // client not found, return false
    return false;
}

bool MqttSNServer::isClientInState(inet::L3Address srcAddress, int srcPort, ClientState clientState)
{
    ClientInfo clientInfo;
    return (isClientExists(srcAddress, srcPort, &clientInfo) && clientInfo.currentState == clientState);
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseEvent);
    cancelAndDelete(activeClientsCheckEvent);
    cancelAndDelete(asleepClientsCheckEvent);
}

} /* namespace mqttsn */
