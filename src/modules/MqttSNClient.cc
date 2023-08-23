#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "types/MsgType.h"
#include "types/Length.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNSearchGw.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNConnect.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        stateChangeEvent = new inet::ClockEvent("stateChangeTimer");
        currentState = ClientState::DISCONNECTED;

        checkGatewaysInterval = par("checkGatewaysInterval");
        checkGatewaysEvent = new inet::ClockEvent("checkGatewaysTimer");

        searchGatewayInterval = uniform(0, par("searchGatewayMaxDelay"));
        searchGatewayEvent = new inet::ClockEvent("searchGatewayTimer");

        temporaryDuration = par("temporaryDuration");

        gatewayInfoInterval = uniform(0, par("gatewayInfoMaxDelay"));
        gatewayInfoEvent = new inet::ClockEvent("gatewayInfoTimer");

        checkConnectionInterval = par("checkConnectionInterval");
        checkConnectionEvent = new inet::ClockEvent("checkConnectionTimer");

        clientId = generateClientId();
    }
}

void MqttSNClient::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == stateChangeEvent) {
        handleStateChangeEvent();
    }
    else if (msg == checkGatewaysEvent) {
        handleCheckGatewaysEvent();
    }
    else if(msg == searchGatewayEvent) {
        handleSearchGatewayEvent();
    }
    else if (msg == gatewayInfoEvent) {
        handleGatewayInfoEvent();
    }
    else if (msg == checkConnectionEvent) {
        handleCheckConnectionEvent();
    }
    else {
        socket.processMessage(msg);
    }
}

void MqttSNClient::finish()
{
    inet::ApplicationBase::finish();
}

void MqttSNClient::refreshDisplay() const
{
    inet::ApplicationBase::refreshDisplay();
}

void MqttSNClient::handleStartOperation(inet::LifecycleOperation *operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    /* TO DO
    scheduleClockEventAt(checkGatewaysInterval, checkGatewaysEvent);
    scheduleClockEventAt(searchGatewayInterval, searchGatewayEvent);
    scheduleClockEventAt(checkConnectionInterval, checkConnectionEvent);
    */

    EV << "Current client state: " << getClientState() << std::endl;

    double currentStateInterval = getStateInterval(currentState);
    if (currentStateInterval != -1) {
        scheduleClockEventAt(currentStateInterval, stateChangeEvent);
    }
}

void MqttSNClient::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(stateChangeEvent);
    cancelEvent(checkGatewaysEvent);
    cancelEvent(searchGatewayEvent);
    cancelEvent(gatewayInfoEvent);
    cancelEvent(checkConnectionEvent);

    socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(stateChangeEvent);
    cancelClockEvent(checkGatewaysEvent);
    cancelClockEvent(searchGatewayEvent);
    cancelClockEvent(gatewayInfoEvent);
    cancelClockEvent(checkConnectionEvent);

    socket.destroy();
}

void MqttSNClient::handleStateChangeEvent()
{
    // get the possible next states based on the current state
    std::vector<ClientState> possibleNextStates = getNextPossibleStates(currentState);

    // randomly select one of the possible next states
    uint16_t nextStateIndex = intuniform(0, possibleNextStates.size() - 1);
    ClientState nextState = possibleNextStates[nextStateIndex];

    // get the interval for the next state
    double nextStateInterval = getStateInterval(nextState);
    if (nextStateInterval != -1) {
        scheduleClockEventAfter(nextStateInterval, stateChangeEvent);
    }

    // perform state transition functions based on current and next states
    performStateTransition(currentState, nextState);

    // update the current state
    currentState = nextState;
    EV << "Current client state: " << getClientState() << std::endl;
}

void MqttSNClient::performStateTransition(ClientState currentState, ClientState nextState)
{
    // calls the appropriate state transition function based on current and next states
    switch (currentState) {
        case ClientState::DISCONNECTED:
            switch (nextState) {
                case ClientState::ACTIVE:
                    fromDisconnectedToActive();
                    break;
                default:
                    break;
            }
            break;

        case ClientState::ACTIVE:
            switch (nextState) {
                case ClientState::DISCONNECTED:
                    fromActiveToDisconnected();
                    break;
                default:
                    break;
            }
            break;

        default:
            break;

        // TO DO -> Add other cases
    }
}

void MqttSNClient::fromDisconnectedToActive()
{
    // TO DO
    EV << "FromDisconnectedToActive" << std::endl;
}

void MqttSNClient::fromActiveToDisconnected()
{
    // TO DO
    EV << "fromActiveToDisconnected" << std::endl;
}

double MqttSNClient::getStateInterval(ClientState currentState)
{
    // returns the interval duration for the given state
    switch (currentState) {
        case ClientState::DISCONNECTED:
            return par("disconnectedStateInterval");

        case ClientState::ACTIVE:
            return par("activeStateInterval");

        case ClientState::LOST:
            return par("lostStateInterval");

        case ClientState::ASLEEP:
            return par("asleepStateInterval");

        case ClientState::AWAKE:
            return par("awakeStateInterval");
    }
}

std::string MqttSNClient::getClientState()
{
    // get current client state as a string
    switch (currentState) {
        case ClientState::DISCONNECTED:
            return "Disconnected";

        case ClientState::ACTIVE:
            return "Active";

        case ClientState::LOST:
            return "Lost";

        case ClientState::ASLEEP:
            return "Asleep";

        case ClientState::AWAKE:
            return "Awake";
    }
}

std::vector<ClientState> MqttSNClient::getNextPossibleStates(ClientState currentState) {
    // get the possible next states based on the current state
    switch (currentState) {
        case ClientState::DISCONNECTED:
            return {ClientState::ACTIVE};

        case ClientState::ACTIVE:
            return {ClientState::LOST, ClientState::ASLEEP, ClientState::DISCONNECTED};

        case ClientState::LOST:
            return {ClientState::ACTIVE};

        case ClientState::ASLEEP:
            return {ClientState::LOST, ClientState::ACTIVE, ClientState::AWAKE};

        case ClientState::AWAKE:
            return {ClientState::ASLEEP, ClientState::ACTIVE, ClientState::DISCONNECTED};
    }
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();

    if (isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

    EV << "Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    const auto& header = pk->peekData<MqttSNBase>();
    checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    switch(header->getMsgType()) {
        // packet types that are allowed only from the selected gateway
        case MsgType::CONNACK:
        case MsgType::WILLTOPICREQ:
        case MsgType::WILLMSGREQ:
            if (!isSelectedGateway(srcAddress, srcPort)) {
                delete pk;
                return;
            }
            break;

        // TO DO -> add the other case where require connection

        default:
            break;
    }

    switch(header->getMsgType()) {
        case MsgType::ADVERTISE:
            processAdvertise(pk, srcAddress, srcPort);
            break;

        case MsgType::SEARCHGW:
            processSearchGw(pk);
            break;

        case MsgType::GWINFO:
            processGwInfo(pk, srcAddress, srcPort);
            break;

        case MsgType::CONNACK:
            processConnAck(pk);
            break;

        case MsgType::WILLTOPICREQ:
            processWillTopicReq(pk, srcAddress, srcPort);
            break;

        case MsgType::WILLMSGREQ:
            processWillMsgReq(pk, srcAddress, srcPort);
            break;

        default:
            break;
    }

    delete pk;
}

void MqttSNClient::processAdvertise(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNAdvertise>();

    uint8_t gatewayId = payload->getGwId();
    uint16_t duration = payload->getDuration();

    updateActiveGateways(gatewayId, duration, srcAddress, srcPort);
}

void MqttSNClient::processSearchGw(inet::Packet *pk)
{
    // no need for this client to send again the search gateway message
    if (searchGateway) {
        searchGateway = false;
    }

    if (!activeGateways.empty()) {
        // delay sending of gwInfo message for a random time
        scheduleClockEventAfter(gatewayInfoInterval, gatewayInfoEvent);
    }
}

void MqttSNClient::processGwInfo(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNGwInfo>();

    uint8_t gatewayId = payload->getGwId();
    std::string gatewayAddress = payload->getGwAdd();
    uint16_t gatewayPort = payload->getGwPort();

    if (gatewayAddress != "" && gatewayPort > 0) {
        // gwInfo from other client
        updateActiveGateways(gatewayId, 0, inet::L3Address(gatewayAddress), (int) gatewayPort);
    }
    else {
        // gwInfo from a server
        updateActiveGateways(gatewayId, 0, srcAddress, srcPort);

        // if client receives a gwInfo message, it will cancel the transmission of its gwInfo message
        cancelEvent(gatewayInfoEvent);
    }

    // completed the search gateway process for the client
    if (searchGateway) {
        searchGateway = false;
    }
}

void MqttSNClient::processConnAck(inet::Packet *pk)
{
    const auto& payload = pk->peekData<MqttSNBaseWithReturnCode>();

    if (payload->getReturnCode() != ReturnCode::ACCEPTED) {
        return;
    }

    // client is connected
    isConnected = true;

    std::ostringstream str;
    str << "Client connected to: " << selectedGateway.address.str() << ":" << selectedGateway.port;
    EV_INFO << str.str() << std::endl;
    bubble(str.str().c_str());
}

void MqttSNClient::processWillTopicReq(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    sendBaseWithWillTopic(MsgType::WILLTOPIC, intToQoS(par("qosFlag")), par("retainFlag"), par("willTopic"), srcAddress, srcPort);
}

void MqttSNClient::processWillMsgReq(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    sendBaseWithWillMsg(MsgType::WILLMSG, par("willMsg"), srcAddress, srcPort);
}

void MqttSNClient::sendSearchGw()
{
    const auto& payload = inet::makeShared<MqttSNSearchGw>();
    payload->setMsgType(MsgType::SEARCHGW);
    payload->setRadius(0x00);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet *packet = new inet::Packet("SearchGwPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
}

void MqttSNClient::sendConnect(bool willFlag, bool cleanSessionFlag, uint16_t duration, inet::L3Address destAddress, int destPort)
{
    const auto& payload = inet::makeShared<MqttSNConnect>();
    payload->setMsgType(MsgType::CONNECT);
    payload->setWillFlag(willFlag);
    payload->setCleanSessionFlag(cleanSessionFlag);
    payload->setDuration(duration);
    payload->setClientId(clientId);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet *packet = new inet::Packet("ConnectPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNClient::sendBaseWithWillTopic(MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic, inet::L3Address destAddress, int destPort)
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

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNClient::sendBaseWithWillMsg(MsgType msgType, std::string willMsg, inet::L3Address destAddress, int destPort)
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

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNClient::handleCheckGatewaysEvent()
{
    checkGatewaysAvailability();
    scheduleClockEventAfter(checkGatewaysInterval, checkGatewaysEvent);
}

void MqttSNClient::handleSearchGatewayEvent()
{
    if (searchGateway) {
        sendSearchGw();

        if (!maxIntervalReached) {
            double maxInterval = par("maxSearchGatewayInterval");

            searchGatewayInterval = std::min(searchGatewayInterval * searchGatewayInterval, maxInterval);
            maxIntervalReached = (searchGatewayInterval == maxInterval);
        }

        scheduleClockEventAfter(searchGatewayInterval, searchGatewayEvent);
    }
}

void MqttSNClient::handleGatewayInfoEvent()
{
    std::pair<uint8_t, GatewayInfo> gateway = selectGateway();

    uint8_t gatewayId = gateway.first;
    GatewayInfo gatewayInfo = gateway.second;

    // client answers with a gwInfo message
    sendGwInfo(gatewayId, gatewayInfo.address.str(), gatewayInfo.port);
}

void MqttSNClient::handleCheckConnectionEvent()
{
    if (!activeGateways.empty() && !isConnected) {
        std::pair<uint8_t, GatewayInfo> gateway = selectGateway();

        GatewayInfo gatewayInfo = gateway.second;
        selectedGateway = gatewayInfo;

        sendConnect(par("willFlag"), par("cleanSessionFlag"), (uint16_t) par("keepAlive"), gatewayInfo.address, gatewayInfo.port);
    }

    scheduleClockEventAfter(checkConnectionInterval, checkConnectionEvent);
}

void MqttSNClient::checkGatewaysAvailability()
{
    int nadv = par("nadv");
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = activeGateways.begin(); it != activeGateways.end();) {

        const GatewayInfo& gatewayInfo = it->second;
        inet::clocktime_t elapsedTime = currentTime - gatewayInfo.lastUpdatedTime;

        if (elapsedTime >= (nadv * gatewayInfo.duration)) {
            // gateway is considered unavailable
            it = activeGateways.erase(it);
        }
        else {
            ++it;
        }
    }
}

void MqttSNClient::updateActiveGateways(uint8_t gatewayId, uint16_t duration, inet::L3Address srcAddress, int srcPort)
{
    auto it = activeGateways.find(gatewayId);

    // gwInfo messages use a temporary duration set in the client
    if (duration == 0) {
        duration = temporaryDuration;
    }

    if (it == activeGateways.end()) {
        GatewayInfo gatewayInfo;
        gatewayInfo.address = srcAddress;
        gatewayInfo.port = srcPort;
        gatewayInfo.duration = duration;
        gatewayInfo.lastUpdatedTime = getClockTime();

        activeGateways[gatewayId] = gatewayInfo;
    }
    else {
        // update the duration field only when we receive an advertise message
        if (duration != temporaryDuration && it->second.duration != duration) {
            it->second.duration = duration;
        }

        it->second.lastUpdatedTime = getClockTime();
    }
}

bool MqttSNClient::isSelectedGateway(inet::L3Address srcAddress, int srcPort)
{
    // check if the gateway with the specified address and port is the one selected by the client
    return (selectedGateway.address == srcAddress && selectedGateway.port == srcPort);
}

bool MqttSNClient::isConnectedGateway(inet::L3Address srcAddress, int srcPort)
{
    // check if the gateway with the specified address and port is the one connected by the client
    return (isConnected && selectedGateway.address == srcAddress && selectedGateway.port == srcPort);
}

std::string MqttSNClient::generateClientId()
{
    // generate a random client ID of variable length
    uint16_t length = intuniform(Length::ONE_OCTET, Length::CLIENT_ID_OCTETS);

    std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string clientId;

    for (uint16_t i = 0; i < length; i++) {
        clientId += allowedChars[intuniform(0, allowedChars.length() - 1)];
    }

    return clientId;
}

std::pair<uint8_t, GatewayInfo> MqttSNClient::selectGateway()
{
    if (activeGateways.empty()) {
        throw omnetpp::cRuntimeError("No active gateway found");
    }

    // random selection policy
    uint16_t index = intuniform(0, activeGateways.size() - 1);

    auto it = activeGateways.begin();
    std::advance(it, index);

    return std::make_pair(it->first, it->second);
}

QoS MqttSNClient::intToQoS(int value)
{
    // convert an integer to QoS enumeration
    switch (value) {
        case 0:
            return QOS_ZERO;

        case 1:
            return QOS_ONE;

        case 2:
            return QOS_TWO;

        case -1:
            return QOS_MINUS_ONE;

        default:
            throw omnetpp::cRuntimeError("Invalid QoS value: %d", value);
    }
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(stateChangeEvent);
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
    cancelAndDelete(gatewayInfoEvent);
    cancelAndDelete(checkConnectionEvent);
}

} /* namespace mqttsn */
