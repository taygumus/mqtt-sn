#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "types/shared/Length.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNSearchGw.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNConnect.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNDisconnect.h"

namespace mqttsn {

void MqttSNClient::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        stateChangeEvent = new inet::ClockEvent("stateChangeTimer");
        currentState = ClientState::DISCONNECTED;

        checkGatewaysInterval = par("checkGatewaysInterval");
        checkGatewaysEvent = new inet::ClockEvent("checkGatewaysTimer");

        searchGatewayMaxDelay = par("searchGatewayMaxDelay");
        searchGatewayInterval = uniform(SEARCH_GATEWAY_MIN_DELAY, searchGatewayMaxDelay);
        searchGatewayEvent = new inet::ClockEvent("searchGatewayTimer");

        temporaryDuration = par("temporaryDuration");

        gatewayInfoMaxDelay = par("gatewayInfoMaxDelay");
        gatewayInfoInterval = uniform(0, gatewayInfoMaxDelay);
        gatewayInfoEvent = new inet::ClockEvent("gatewayInfoTimer");

        checkConnectionInterval = par("checkConnectionInterval");
        checkConnectionEvent = new inet::ClockEvent("checkConnectionTimer");

        clientId = generateClientId();

        keepAlive = par("keepAlive");
        pingEvent = new inet::ClockEvent("pingTimer");

        retransmissionInterval = par("retransmissionInterval");

        initializeCustom();
    }
}

void MqttSNClient::handleMessageWhenUp(omnetpp::cMessage* msg)
{
    if (msg == stateChangeEvent) {
        handleStateChangeEvent();
    }
    else if (msg->hasPar("isRetransmissionEvent")) {
        handleRetransmissionEvent(msg);
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
    else if (msg == pingEvent) {
        handlePingEvent();
    }
    else if (!handleMessageWhenUpCustom(msg)) {
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

void MqttSNClient::handleStartOperation(inet::LifecycleOperation* operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char* localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    EV << "Current client state: " << getClientStateAsString() << std::endl;

    double currentStateInterval = getStateInterval(currentState);
    if (currentStateInterval != -1) {
        scheduleClockEventAt(currentStateInterval, stateChangeEvent);
    }
}

void MqttSNClient::handleStopOperation(inet::LifecycleOperation* operation)
{
    cancelEvent(stateChangeEvent);
    cancelActiveStateEvents();
    clearRetransmissions();

    socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation* operation)
{
    cancelActiveStateClockEvents();
    clearRetransmissions();

    socket.destroy();
}

void MqttSNClient::handleStateChangeEvent()
{
    // get the possible next states based on the current state
    std::vector<ClientState> possibleNextStates = getNextPossibleStates(currentState);

    // randomly select one of the possible next states
    uint16_t nextStateIndex = intuniform(0, possibleNextStates.size() - 1);
    ClientState nextState = possibleNextStates[nextStateIndex];

    // perform state transition functions based on the current and next states and return true if the transition is successful
    if (performStateTransition(currentState, nextState)) {
        // get the interval for the next state
        double nextStateInterval = getStateInterval(nextState);
        if (nextStateInterval != -1) {
            scheduleClockEventAfter(nextStateInterval, stateChangeEvent);
        }

        updateCurrentState(nextState);
    }
}

void MqttSNClient::updateCurrentState(ClientState nextState)
{
    currentState = nextState;
    EV << "Current client state: " << getClientStateAsString() << std::endl;
}

void MqttSNClient::returnToSleep()
{
    // transition to ASLEEP state
    EV << "Awake -> Asleep" << std::endl;
    updateCurrentState(ClientState::ASLEEP);

    scheduleClockEventAfter(getStateInterval(currentState), stateChangeEvent);
}

void MqttSNClient::scheduleActiveStateEvents()
{
    searchGatewayInterval = uniform(SEARCH_GATEWAY_MIN_DELAY, searchGatewayMaxDelay);
    gatewayInfoInterval = uniform(0, gatewayInfoMaxDelay);

    activeGateways.clear();

    maxIntervalReached = false;
    searchGateway = true;
    isConnected = false;

    scheduleClockEventAfter(checkGatewaysInterval, checkGatewaysEvent);
    scheduleClockEventAfter(searchGatewayInterval, searchGatewayEvent);
    scheduleClockEventAfter(checkConnectionInterval, checkConnectionEvent);

    scheduleActiveStateEventsCustom();
}

void MqttSNClient::cancelActiveStateEvents()
{
    cancelEvent(checkGatewaysEvent);
    cancelEvent(searchGatewayEvent);
    cancelEvent(gatewayInfoEvent);
    cancelEvent(checkConnectionEvent);
    cancelEvent(pingEvent);

    cancelActiveStateEventsCustom();
}

void MqttSNClient::cancelActiveStateClockEvents()
{
    cancelClockEvent(stateChangeEvent);
    cancelClockEvent(checkGatewaysEvent);
    cancelClockEvent(searchGatewayEvent);
    cancelClockEvent(gatewayInfoEvent);
    cancelClockEvent(checkConnectionEvent);
    cancelClockEvent(pingEvent);

    cancelActiveStateClockEventsCustom();
}

bool MqttSNClient::fromDisconnectedToActive()
{
    EV << "Disconnected -> Active" << std::endl;
    scheduleActiveStateEvents();

    return true;
}

bool MqttSNClient::fromLostToActive()
{
    EV << "Lost -> Active" << std::endl;
    scheduleActiveStateEvents();

    return true;
}

bool MqttSNClient::fromActiveToDisconnected()
{
    if (!isConnected) {
        EV << "Active -> Disconnected" << std::endl;
        cancelActiveStateEvents();

        return true;
    }

    MqttSNApp::sendDisconnect(selectedGateway.address, selectedGateway.port);

    // schedule disconnect retransmission
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::DISCONNECT);

    return false;
}

bool MqttSNClient::fromActiveToLost()
{
    EV << "Active -> Lost" << std::endl;
    cancelActiveStateEvents();
    clearRetransmissions();

    return true;
}

bool MqttSNClient::fromActiveToAsleep()
{
    if (!isConnected) {
        EV << "Active -> Asleep" << std::endl;
        cancelActiveStateEvents();

        return true;
    }

    double asleepStateInterval = par("asleepStateInterval");
    uint16_t sleepDuration = 0;

    if (asleepStateInterval == -1) {
        // if the interval is -1, set duration to the maximum value for uint16_t
        sleepDuration = UINT16_MAX;
    }
    else if (asleepStateInterval > 0 && asleepStateInterval <= UINT16_MAX) {
        // if the interval is within the valid range, proceed
        sleepDuration = std::round(asleepStateInterval);
    }
    else {
        throw omnetpp::cRuntimeError("Invalid asleep state interval value");
    }

    MqttSNApp::sendDisconnect(selectedGateway.address, selectedGateway.port, sleepDuration);

    // schedule disconnect retransmission
    std::map<std::string, std::string> parameters;
    parameters["sleepDuration"] = std::to_string(sleepDuration);
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::DISCONNECT, &parameters);

    return false;
}

bool MqttSNClient::fromAsleepToLost()
{
    EV << "Asleep -> Lost" << std::endl;
    cancelActiveStateEvents();
    clearRetransmissions();

    return true;
}

bool MqttSNClient::fromAsleepToActive()
{
    EV << "Asleep -> Active" << std::endl;
    scheduleActiveStateEvents();
    clearRetransmissions();

    return true;
}

bool MqttSNClient::fromAsleepToAwake()
{
    if (!isConnected) {
        // remain in ASLEEP state
        scheduleClockEventAfter(MIN_WAITING_TIME, stateChangeEvent);

        return false;
    }

    clearRetransmissions();

    EV << "Asleep -> Awake" << std::endl;
    MqttSNApp::sendPingReq(selectedGateway.address, selectedGateway.port, clientId);

    // schedule ping retransmission
    std::map<std::string, std::string> parameters;
    parameters["clientId"] = clientId;
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::PINGREQ, &parameters);

    return true;
}

bool MqttSNClient::fromAsleepToDisconnected()
{
    if (!isConnected) {
        EV << "Asleep -> Disconnected" << std::endl;
        cancelActiveStateEvents();

        return true;
    }

    MqttSNApp::sendDisconnect(selectedGateway.address, selectedGateway.port);

    // schedule disconnect retransmission
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::DISCONNECT);

    return false;
}

bool MqttSNClient::performStateTransition(ClientState currentState, ClientState nextState)
{
    // calls the appropriate state transition function based on current and next states
    switch (currentState) {
        case ClientState::DISCONNECTED:
            switch (nextState) {
                case ClientState::ACTIVE:
                    return fromDisconnectedToActive();
                default:
                    break;
            }
            break;

        case ClientState::ACTIVE:
            switch (nextState) {
                case ClientState::DISCONNECTED:
                    return fromActiveToDisconnected();
                case ClientState::LOST:
                    return fromActiveToLost();
                case ClientState::ASLEEP:
                    return fromActiveToAsleep();
                default:
                    break;
            }
            break;

        case ClientState::LOST:
            switch (nextState) {
                case ClientState::ACTIVE:
                    return fromLostToActive();
                default:
                    break;
            }
            break;

       case ClientState::ASLEEP:
           switch (nextState) {
               case ClientState::LOST:
                   return fromAsleepToLost();
               case ClientState::ACTIVE:
                   return fromAsleepToActive();
               case ClientState::AWAKE:
                   return fromAsleepToAwake();
               case ClientState::DISCONNECTED:
                   return fromAsleepToDisconnected();
               default:
                   break;
           }
           break;

        default:
            break;
    }

    return false;
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
            return -1;
    }
}

std::string MqttSNClient::getClientStateAsString()
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
            return {ClientState::LOST, ClientState::ACTIVE, ClientState::AWAKE, ClientState::DISCONNECTED};

        default:
            return {ClientState::DISCONNECTED};
    }
}

void MqttSNClient::processPacket(inet::Packet* pk)
{
    if (currentState == ClientState::DISCONNECTED || currentState == ClientState::LOST) {
        delete pk;
        return;
    }

    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();

    if (MqttSNApp::isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

    const auto& header = pk->peekData<MqttSNBase>();

    MqttSNApp::checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());
    MsgType msgType = header->getMsgType();

    // delete packet if client is ASLEEP and message type is not DISCONNECT
    if (currentState == ClientState::ASLEEP && msgType != MsgType::DISCONNECT) {
        delete pk;
        return;
    }

    std::vector<MsgType> allowedAwakeMsgTypes = {MsgType::PINGRESP};
    // TO DO -> pure virtual function -> the subscriber implementation will add into the vector the PUBLISH message
    if (currentState == ClientState::AWAKE &&
        std::find(allowedAwakeMsgTypes.begin(), allowedAwakeMsgTypes.end(), msgType) == allowedAwakeMsgTypes.end()) {
        // delete the packet if the message type is not in the allowed list while the client is AWAKE
        delete pk;
        return;
    }

    EV << "Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    switch(msgType) {
        // packet types that are allowed only from the selected gateway
        case MsgType::CONNACK:
            if (!isSelectedGateway(srcAddress, srcPort)) {
                delete pk;
                return;
            }
            break;

        // packet types that are allowed only from the connected gateway
        case MsgType::PINGREQ:
        case MsgType::PINGRESP:
        case MsgType::DISCONNECT:
            if (!isConnectedGateway(srcAddress, srcPort)) {
                delete pk;
                return;
            }
            break;

        default:
            break;
    }

    switch(msgType) {
        case MsgType::ADVERTISE:
            processAdvertise(pk, srcAddress, srcPort);
            break;

        case MsgType::SEARCHGW:
            processSearchGw();
            break;

        case MsgType::GWINFO:
            processGwInfo(pk, srcAddress, srcPort);
            break;

        case MsgType::CONNACK:
            processConnAck(pk);
            break;

        case MsgType::PINGREQ:
            processPingReq(srcAddress, srcPort);
            break;

        case MsgType::PINGRESP:
            processPingResp(srcAddress, srcPort);
            break;

        case MsgType::DISCONNECT:
            processDisconnect(pk);
            break;

        default:
            break;
    }

    processPacketCustom(pk, srcAddress, srcPort, msgType);

    delete pk;
}

void MqttSNClient::processAdvertise(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNAdvertise>();

    uint8_t gatewayId = payload->getGwId();
    uint16_t duration = payload->getDuration();

    updateActiveGateways(srcAddress, srcPort, gatewayId, duration);
}

void MqttSNClient::processSearchGw()
{
    // no need for this client to send again the search gateway message
    if (searchGateway) {
        searchGateway = false;
    }

    if (!activeGateways.empty() && !gatewayInfoEvent->isScheduled()) {
        // delay sending of gwInfo message for a random time
        scheduleClockEventAfter(gatewayInfoInterval, gatewayInfoEvent);
    }
}

void MqttSNClient::processGwInfo(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNGwInfo>();

    uint8_t gatewayId = payload->getGwId();
    std::string gatewayAddress = payload->getGwAdd();
    uint16_t gatewayPort = payload->getGwPort();

    if (!gatewayAddress.empty() && gatewayPort > 0) {
        // gwInfo from other client
        inet::L3Address ipAddress = inet::L3AddressResolver().resolve(gatewayAddress.c_str());
        updateActiveGateways(ipAddress, (int) gatewayPort, gatewayId, 0);
    }
    else {
        // gwInfo from a server
        updateActiveGateways(srcAddress, srcPort, gatewayId, 0);

        // if client receives a gwInfo message, it will cancel the transmission of its gwInfo message
        cancelEvent(gatewayInfoEvent);
    }

    // completed the search gateway process for the client
    if (searchGateway) {
        searchGateway = false;
    }
}

void MqttSNClient::processConnAck(inet::Packet* pk)
{
    const auto& payload = pk->peekData<MqttSNBaseWithReturnCode>();

    if (payload->getReturnCode() != ReturnCode::ACCEPTED) {
        return;
    }

    // client is connected
    isConnected = true;

    // reschedule the ping event
    cancelEvent(pingEvent);
    scheduleClockEventAfter(keepAlive, pingEvent);

    std::ostringstream str;
    str << "Client connected to: " << selectedGateway.address.str() << ":" << selectedGateway.port;
    EV << str.str() << std::endl;
}

void MqttSNClient::processPingReq(const inet::L3Address& srcAddress, const int& srcPort)
{
    MqttSNApp::sendBase(srcAddress, srcPort, MsgType::PINGRESP);
}

void MqttSNClient::processPingResp(const inet::L3Address& srcAddress, const int& srcPort)
{
    if (currentState == ClientState::AWAKE) {
        returnToSleep();
        clearRetransmissions();
    }
    else {
        EV << "Received ping response from server: " << srcAddress << ":" << srcPort << std::endl;
        unscheduleMsgRetransmission(MsgType::PINGREQ);
    }
}

void MqttSNClient::processDisconnect(inet::Packet* pk)
{
    const auto& payload = pk->peekData<MqttSNDisconnect>();
    uint16_t sleepDuration = payload->getDuration();

    double nextStateInterval;

    if (sleepDuration > 0) {
        // message with duration field
        EV << "Active -> Asleep" << std::endl;
        cancelActiveStateEvents();

        nextStateInterval = getStateInterval(ClientState::ASLEEP);
        updateCurrentState(ClientState::ASLEEP);
    }
    else {
        // message without duration field
        if (currentState == ClientState::ASLEEP) {
            EV << "Asleep -> Disconnected" << std::endl;
        }
        else {
            EV << "Active -> Disconnected" << std::endl;
            cancelActiveStateEvents();
        }

        nextStateInterval = getStateInterval(ClientState::DISCONNECTED);
        updateCurrentState(ClientState::DISCONNECTED);
    }

    if (nextStateInterval != -1) {
        scheduleClockEventAfter(nextStateInterval, stateChangeEvent);
    }

    clearRetransmissions();
}

void MqttSNClient::sendSearchGw()
{
    const auto& payload = inet::makeShared<MqttSNSearchGw>();
    payload->setMsgType(MsgType::SEARCHGW);
    payload->setRadius(0x00);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("SearchGwPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
}

void MqttSNClient::sendConnect(const inet::L3Address& destAddress, const int& destPort, bool willFlag, bool cleanSessionFlag, uint16_t duration)
{
    const auto& payload = inet::makeShared<MqttSNConnect>();
    payload->setMsgType(MsgType::CONNECT);
    payload->setWillFlag(willFlag);
    payload->setCleanSessionFlag(cleanSessionFlag);
    payload->setDuration(duration);
    payload->setClientId(clientId);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("ConnectPacket");
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNClient::handleCheckGatewaysEvent()
{
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = activeGateways.begin(); it != activeGateways.end();) {
        const GatewayInfo& gatewayInfo = it->second;

        // check if the elapsed time exceeds the threshold
        if ((currentTime - gatewayInfo.lastUpdatedTime) > ((int) par("nadv")*  gatewayInfo.duration)) {
            // gateway is considered unavailable
            it = activeGateways.erase(it);
        }
        else {
            ++it;
        }
    }

    scheduleClockEventAfter(checkGatewaysInterval, checkGatewaysEvent);
}

void MqttSNClient::handleSearchGatewayEvent()
{
    // check if a search for gateways is needed
    if (!searchGateway) {
        return;
    }

    // send a search gateway message
    sendSearchGw();

    if (!maxIntervalReached) {
        double maxInterval = par("maxSearchGatewayInterval");

        // increase search interval exponentially, with a maximum limit
        searchGatewayInterval = std::min(searchGatewayInterval*  searchGatewayInterval, maxInterval);
        maxIntervalReached = (searchGatewayInterval == maxInterval);
    }

    scheduleClockEventAfter(searchGatewayInterval, searchGatewayEvent);
}

void MqttSNClient::handleGatewayInfoEvent()
{
    if (activeGateways.empty()) {
        return;
    }

    std::pair<uint8_t, GatewayInfo> gateway = selectGateway();

    uint8_t gatewayId = gateway.first;
    GatewayInfo gatewayInfo = gateway.second;

    // client answers with a gwInfo message
    MqttSNApp::sendGwInfo(gatewayId, gatewayInfo.address.str(), gatewayInfo.port);
}

void MqttSNClient::handleCheckConnectionEvent()
{
    if (isConnected) {
        return;
    }

    // if there are active gateways, select one and handle the connection event
    if (!activeGateways.empty()) {
        std::pair<uint8_t, GatewayInfo> gateway = selectGateway();

        GatewayInfo gatewayInfo = gateway.second;
        selectedGateway = gatewayInfo;

        handleCheckConnectionEventCustom(gatewayInfo.address, gatewayInfo.port);
    }

    scheduleClockEventAfter(checkConnectionInterval, checkConnectionEvent);
}

void MqttSNClient::handlePingEvent()
{
    MqttSNApp::sendPingReq(selectedGateway.address, selectedGateway.port);

    // schedule ping retransmission
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::PINGREQ);

    scheduleClockEventAfter(keepAlive, pingEvent);
}

void MqttSNClient::updateActiveGateways(const inet::L3Address& srcAddress, const int& srcPort, uint8_t gatewayId, uint16_t duration)
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

bool MqttSNClient::isSelectedGateway(const inet::L3Address& srcAddress, const int& srcPort)
{
    // check if the gateway with the specified address and port is the one selected by the client
    return (selectedGateway.address == srcAddress && selectedGateway.port == srcPort);
}

bool MqttSNClient::isConnectedGateway(const inet::L3Address& srcAddress, const int& srcPort)
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

void MqttSNClient::scheduleMsgRetransmission(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, std::map<std::string, std::string>* parameters)
{
    // check if a message of the same type is already scheduled for retransmission
    if (retransmissions.find(msgType) != retransmissions.end()) {
        // exit without doing anything
        return;
    }

    // create a new structure for this message type
    UnicastMessageInfo newInfo;
    newInfo.retransmissionEvent = new inet::ClockEvent("retransmissionTimer");
    newInfo.retransmissionCounter = 0;
    newInfo.destAddress = destAddress;
    newInfo.destPort = destPort;

    // add other dynamic parameters to the timer
    if (parameters != nullptr) {
        for (const auto& param : *parameters) {
            newInfo.retransmissionEvent->addPar(param.first.c_str()).setStringValue(param.second.c_str());
        }
    }

    // flag to identify this event as a retransmission message
    newInfo.retransmissionEvent->addPar("isRetransmissionEvent");
    // set the message type as a parameter
    newInfo.retransmissionEvent->addPar("messageType").setLongValue(static_cast<long>(msgType));

    // add the timer and information to the retransmissions map
    retransmissions[msgType] = newInfo;

    // start the timer
    scheduleClockEventAfter(retransmissionInterval, newInfo.retransmissionEvent);
}

void MqttSNClient::unscheduleMsgRetransmission(MsgType msgType)
{
    // find the element in the map with the specified message type
    auto it = retransmissions.find(msgType);

    // check if the element is found in the map
    if (it != retransmissions.end()) {
        UnicastMessageInfo& unicastMessageInfo = it->second;
        // cancel the event inside the struct
        cancelAndDelete(unicastMessageInfo.retransmissionEvent);

        // remove the element from the map
        retransmissions.erase(it);
    }
}

void MqttSNClient::clearRetransmissions()
{
    // clear the map to remove all elements
    for (auto it = retransmissions.begin(); it != retransmissions.end(); ++it) {
        // cancel all associated events in the map
        cancelAndDelete(it->second.retransmissionEvent);
        retransmissions.erase(it);
    }
}

void MqttSNClient::handleRetransmissionEvent(omnetpp::cMessage* msg)
{
    // get the message type from the message parameter
    MsgType msgType = static_cast<MsgType>(msg->par("messageType").longValue());

    auto it = retransmissions.find(msgType);

    // check the message type in the map
    if (it == retransmissions.end()) {
        // if not found, exit the function
        return;
    }

    UnicastMessageInfo* unicastMessageInfo = &it->second;

    // check if the number of retries equals the threshold
    if (unicastMessageInfo->retransmissionCounter >= par("retransmissionCounter").intValue()) {
        // stop further retransmissions and perform state transition
        if (currentState == ClientState::AWAKE) {
            returnToSleep();
        }
        else {
            // if the client is in an ACTIVE state, reset state events
            if (currentState == ClientState::ACTIVE) {
                cancelActiveStateEvents();
            }

            updateCurrentState(ClientState::DISCONNECTED);
            cancelEvent(stateChangeEvent);
            scheduleClockEventAfter(MIN_WAITING_TIME, stateChangeEvent);
        }

        clearRetransmissions();
        return;
    }

    switch (msgType) {
        case MsgType::DISCONNECT:
            retransmitDisconnect(unicastMessageInfo->destAddress, unicastMessageInfo->destPort, msg);
            break;

        case MsgType::PINGREQ:
            retransmitPingReq(unicastMessageInfo->destAddress, unicastMessageInfo->destPort, msg);
            break;

        default:
            break;
    }

    handleRetransmissionEventCustom(unicastMessageInfo->destAddress, unicastMessageInfo->destPort, msg, msgType);

    unicastMessageInfo->retransmissionCounter++;
    scheduleClockEventAfter(retransmissionInterval, unicastMessageInfo->retransmissionEvent);
}

void MqttSNClient::retransmitDisconnect(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    if (msg->hasPar("sleepDuration")) {
        MqttSNApp::sendDisconnect(destAddress, destPort, std::stoul(msg->par("sleepDuration").stringValue()));
    }
    else {
        MqttSNApp::sendDisconnect(destAddress, destPort);
    }
}

void MqttSNClient::retransmitPingReq(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    if (msg->hasPar("clientId")) {
        MqttSNApp::sendPingReq(destAddress, destPort, msg->par("clientId").stringValue());
    }
    else {
        MqttSNApp::sendPingReq(destAddress, destPort);
    }
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(stateChangeEvent);
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
    cancelAndDelete(gatewayInfoEvent);
    cancelAndDelete(checkConnectionEvent);
    cancelAndDelete(pingEvent);

    clearRetransmissions();
}

} /* namespace mqttsn */
