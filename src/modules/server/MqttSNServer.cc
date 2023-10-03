#include "MqttSNServer.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNConnect.h"
#include "messages/MqttSNBase.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"
#include "messages/MqttSNDisconnect.h"
#include "messages/MqttSNPingReq.h"

namespace mqttsn {

Define_Module(MqttSNServer);

int MqttSNServer::gatewayIdCounter = -1;

void MqttSNServer::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        stateChangeEvent = new inet::ClockEvent("stateChangeTimer");
        currentState = GatewayState::OFFLINE;

        advertiseInterval = par("advertiseInterval");
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

        clientsClearInterval = par("clientsClearInterval");
        clientsClearEvent = new inet::ClockEvent("clientsClearTimer");
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage* msg)
{
    if (msg == stateChangeEvent) {
        handleStateChangeEvent();
    }
    else if (msg == advertiseEvent) {
        handleAdvertiseEvent();
    }
    else if (msg == activeClientsCheckEvent) {
        handleActiveClientsCheckEvent();
    }
    else if (msg == asleepClientsCheckEvent) {
        handleAsleepClientsCheckEvent();
    }
    else if (msg == clientsClearEvent) {
        handleClientsClearEvent();
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

void MqttSNServer::handleStartOperation(inet::LifecycleOperation* operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char* localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    EV << "Current gateway state: " << getGatewayStateAsString() << std::endl;

    double currentStateInterval = getStateInterval(currentState);
    if (currentStateInterval != -1) {
        scheduleClockEventAt(currentStateInterval, stateChangeEvent);
    }
}

void MqttSNServer::handleStopOperation(inet::LifecycleOperation* operation)
{
    cancelEvent(stateChangeEvent);
    cancelOnlineStateEvents();

    socket.close();
}

void MqttSNServer::handleCrashOperation(inet::LifecycleOperation* operation)
{
    cancelClockEvent(stateChangeEvent);
    cancelClockEvent(advertiseEvent);
    cancelClockEvent(activeClientsCheckEvent);
    cancelClockEvent(asleepClientsCheckEvent);
    cancelClockEvent(clientsClearEvent);

    socket.destroy();
}

void MqttSNServer::handleStateChangeEvent()
{
    // get the possible next state based on the current state
    GatewayState nextState = (currentState == GatewayState::ONLINE) ? GatewayState::OFFLINE : GatewayState::ONLINE;

    // get the interval for the next state
    double nextStateInterval = getStateInterval(nextState);
    if (nextStateInterval != -1) {
        scheduleClockEventAfter(nextStateInterval, stateChangeEvent);
    }

    // perform state transition and update if successful
    if (performStateTransition(currentState, nextState)) {
        updateCurrentState(nextState);
    }
}

void MqttSNServer::scheduleOnlineStateEvents()
{
    scheduleClockEventAfter(advertiseInterval, advertiseEvent);
    scheduleClockEventAfter(activeClientsCheckInterval, activeClientsCheckEvent);
    scheduleClockEventAfter(asleepClientsCheckInterval, asleepClientsCheckEvent);
    scheduleClockEventAfter(clientsClearInterval, clientsClearEvent);
}

void MqttSNServer::cancelOnlineStateEvents()
{
    cancelEvent(advertiseEvent);
    cancelEvent(activeClientsCheckEvent);
    cancelEvent(asleepClientsCheckEvent);
    cancelEvent(clientsClearEvent);
}

void MqttSNServer::updateCurrentState(GatewayState nextState)
{
    currentState = nextState;
    EV << "Current gateway state: " << getGatewayStateAsString() << std::endl;
}

bool MqttSNServer::fromOfflineToOnline()
{
    EV << "Offline -> Online" << std::endl;
    scheduleOnlineStateEvents();

    return true;
}

bool MqttSNServer::fromOnlineToOffline()
{
    EV << "Online -> Offline" << std::endl;
    cancelOnlineStateEvents();

    return true;
}

bool MqttSNServer::performStateTransition(GatewayState currentState, GatewayState nextState) {
    // determine the transition function based on current and next states
    switch (currentState) {
        case GatewayState::OFFLINE:
            if (nextState == GatewayState::ONLINE) {
                return fromOfflineToOnline();
            }
            break;

        case GatewayState::ONLINE:
            if (nextState == GatewayState::OFFLINE) {
                return fromOnlineToOffline();
            }
            break;

        default:
            break;
    }

    return false;
}


double MqttSNServer::getStateInterval(GatewayState currentState)
{
    // returns the interval duration for the given state
    switch (currentState) {
        case GatewayState::OFFLINE:
            return par("offlineStateInterval");

        case GatewayState::ONLINE:
            return par("onlineStateInterval");
    }
}

std::string MqttSNServer::getGatewayStateAsString()
{
    // get current gateway state as a string
    switch (currentState) {
        case GatewayState::OFFLINE:
            return "Offline";

        case GatewayState::ONLINE:
            return "Online";
    }
}

void MqttSNServer::processPacket(inet::Packet* pk)
{
    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();

    if (currentState == GatewayState::OFFLINE || MqttSNApp::isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

    EV << "Server received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    const auto& header = pk->peekData<MqttSNBase>();

    MqttSNApp::checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());
    MsgType msgType = header->getMsgType();

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    switch(msgType) {
        // packet types that require an ACTIVE client state
        case MsgType::WILLTOPIC:
        case MsgType::WILLMSG:
        case MsgType::PINGRESP:
            if (!isClientInState(srcAddress, srcPort, ClientState::ACTIVE)) {
                delete pk;
                return;
            }
            break;

        // packet types that require an ACTIVE or ASLEEP client state
        case MsgType::PINGREQ:
        case MsgType::DISCONNECT:
            if (!isClientInState(srcAddress, srcPort, ClientState::ACTIVE) &&
                !isClientInState(srcAddress, srcPort, ClientState::ASLEEP)) {
                delete pk;
                return;
            }
            break;

        default:
            break;
    }

    switch(msgType) {
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
            processPingReq(pk, srcAddress, srcPort);
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

void MqttSNServer::processConnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNConnect>();

    // prevent client connection when its protocol ID is not supported
    if (payload->getProtocolId() != 0x01) {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::REJECTED_NOT_SUPPORTED);
        return;
    }

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);

    // prevent new client connections when the gateway is congested
    if (isGatewayCongested() && clientInfo->isNew) {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::REJECTED_CONGESTION);
        return;
    }

    bool willFlag = payload->getWillFlag();

    // update client information
    clientInfo->isNew = false;
    clientInfo->clientId = payload->getClientId();
    clientInfo->keepAliveDuration = payload->getDuration();
    ///clientInfo->cleanSessionFlag = payload->getCleanSessionFlag();
    clientInfo->currentState = ClientState::ACTIVE;
    clientInfo->lastReceivedMsgTime = getClockTime();

    if (willFlag) {
        PublisherInfo* publisherInfo = getPublisherInfo(srcAddress, srcPort, true);
        // update publisher information
        publisherInfo->willFlag = willFlag;

        MqttSNApp::sendBase(srcAddress, srcPort, MsgType::WILLTOPICREQ);
    }
    else {
        sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::ACCEPTED);
    }
}

void MqttSNServer::processWillTopic(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithWillTopic>();

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);
    // update client information
    clientInfo->lastReceivedMsgTime = getClockTime();

    PublisherInfo* publisherInfo = getPublisherInfo(srcAddress, srcPort, true);
    // update publisher information
    publisherInfo->willQosFlag = (QoS) payload->getQoSFlag();
    publisherInfo->willRetainFlag = payload->getRetainFlag();
    publisherInfo->willTopic = payload->getWillTopic();

    MqttSNApp::sendBase(srcAddress, srcPort, MsgType::WILLMSGREQ);
}

void MqttSNServer::processWillMsg(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithWillMsg>();

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);
    // update client information
    clientInfo->lastReceivedMsgTime = getClockTime();

    PublisherInfo* publisherInfo = getPublisherInfo(srcAddress, srcPort, true);
    // update publisher information
    publisherInfo->willMsg = payload->getWillMsg();

    sendBaseWithReturnCode(srcAddress, srcPort, MsgType::CONNACK, ReturnCode::ACCEPTED);
}

void MqttSNServer::processPingReq(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNPingReq>();
    std::string clientId = payload->getClientId();

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);
    // update client information
    clientInfo->lastReceivedMsgTime = getClockTime();

    if (!clientId.empty()) {
        // check if the client ID matches the expected client ID
        if (clientInfo->clientId != clientId) {
            return;
        }

        clientInfo->currentState = ClientState::AWAKE;

        // TO DO -> send buffered messages to the client
        // TO DO -> this part can be improved e.g. check if there are buffered messages otherwise do not change state

        clientInfo->currentState = ClientState::ASLEEP;
    }

    MqttSNApp::sendBase(srcAddress, srcPort, MsgType::PINGRESP);
}

void MqttSNServer::processPingResp(const inet::L3Address& srcAddress, const int& srcPort)
{
    EV << "Received ping response from client: " << srcAddress << ":" << srcPort << std::endl;

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);
    // update client information
    clientInfo->lastReceivedMsgTime = getClockTime();
    clientInfo->sentPingReq = false;
}

void MqttSNServer::processDisconnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNDisconnect>();
    uint16_t sleepDuration = payload->getDuration();

    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort, true);
    // update client information
    clientInfo->sleepDuration = sleepDuration;
    clientInfo->currentState = (sleepDuration > 0) ? ClientState::ASLEEP : ClientState::DISCONNECTED;
    clientInfo->lastReceivedMsgTime = getClockTime();

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
    inet::Packet* packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
    numAdvertiseSent++;
}

void MqttSNServer::sendBaseWithReturnCode(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, ReturnCode returnCode)
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

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNServer::handleAdvertiseEvent()
{
    sendAdvertise();

    scheduleClockEventAfter(advertiseInterval, advertiseEvent);
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

void MqttSNServer::handleClientsClearEvent()
{
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = clients.begin(); it != clients.end();) {
        ClientInfo& clientInfo = it->second;

        // check if the client is LOST or DISCONNECTED and if the elapsed time exceeds the threshold
        if ((clientInfo.currentState == ClientState::LOST || clientInfo.currentState == ClientState::DISCONNECTED) &&
            (currentTime - clientInfo.lastReceivedMsgTime) > par("maximumInactivityTime")) {

            it = clients.erase(it);
        }
        else {
            ++it;
        }
    }

    scheduleClockEventAfter(clientsClearInterval, clientsClearEvent);
}

bool MqttSNServer::isGatewayCongested()
{
    // check for gateway congestion based on clients count
    return clients.size() >= (unsigned int) par("maximumClients");
}

bool MqttSNServer::isClientInState(const inet::L3Address& srcAddress, const int& srcPort, ClientState clientState)
{
    // get client information with the specified IP address and port
    ClientInfo* clientInfo = getClientInfo(srcAddress, srcPort);

    // return true if the client is found and its state matches the requested state, otherwise return false
    return (clientInfo != nullptr && clientInfo->currentState == clientState);
}

ClientInfo* MqttSNServer::getClientInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound)
{
    // check if the client with the specified address and port is present in the data structure
    auto clientIterator = clients.find(std::make_pair(srcAddress, srcPort));

    if (clientIterator != clients.end()) {
        return &clientIterator->second;
    }

    if (insertIfNotFound) {
        // insert a new empty client
        ClientInfo newClientInfo;
        clients[std::make_pair(srcAddress, srcPort)] = newClientInfo;

        return &clients[std::make_pair(srcAddress, srcPort)];
    }

    return nullptr;
}

PublisherInfo* MqttSNServer::getPublisherInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound)
{
    // check if the publisher with the specified address and port is present in the data structure
    auto publisherIterator = publishers.find(std::make_pair(srcAddress, srcPort));

    if (publisherIterator != publishers.end()) {
        return &publisherIterator->second;
    }

    if (insertIfNotFound) {
        // insert a new empty publisher
        PublisherInfo newPublisherInfo;
        publishers[std::make_pair(srcAddress, srcPort)] = newPublisherInfo;

        return &publishers[std::make_pair(srcAddress, srcPort)];
    }

    return nullptr;
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(stateChangeEvent);
    cancelAndDelete(advertiseEvent);
    cancelAndDelete(activeClientsCheckEvent);
    cancelAndDelete(asleepClientsCheckEvent);
    cancelAndDelete(clientsClearEvent);
}

} /* namespace mqttsn */
