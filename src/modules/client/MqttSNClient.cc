//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "helpers/StringHelper.h"
#include "types/shared/Length.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNSearchGw.h"
#include "messages/MqttSNGwInfo.h"
#include "messages/MqttSNConnect.h"
#include "messages/MqttSNBaseWithReturnCode.h"
#include "messages/MqttSNDisconnect.h"
#include <fstream>

namespace mqttsn {

const std::string MqttSNClient::TOPIC_DELIMITER = "-";

double MqttSNClient::sumReceivedPublishMsgTimestamps;
unsigned MqttSNClient::receivedTotalPublishMsgs;

unsigned MqttSNClient::sentUniquePublishMsgs = 0;
unsigned MqttSNClient::receivedUniquePublishMsgs = 0;
unsigned MqttSNClient::receivedDuplicatePublishMsgs = 0;

unsigned MqttSNClient::publishersRetransmissions = 0;
unsigned MqttSNClient::subscribersRetransmissions = 0;

void MqttSNClient::levelOneInit()
{
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

    waitingInterval = par("waitingInterval");

    predefinedTopics = MqttSNApp::getPredefinedTopics();

    sumReceivedPublishMsgTimestamps = 0;
    receivedTotalPublishMsgs = 0;

    sentUniquePublishMsgs = 0;
    receivedUniquePublishMsgs = 0;
    receivedDuplicatePublishMsgs = 0;

    publishersRetransmissions = 0;
    subscribersRetransmissions = 0;

    levelTwoInit();
}

void MqttSNClient::finish()
{
    handleFinalSimulationResults();

    MqttSNApp::finish();
}

void MqttSNClient::handleStartOperation(inet::LifecycleOperation* operation)
{
    MqttSNApp::socketConfiguration();

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

    MqttSNApp::socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation* operation)
{
    cancelActiveStateClockEvents();
    clearRetransmissions();

    MqttSNApp::socket.destroy();
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
        MqttSNApp::socket.processMessage(msg);
    }
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

    gateways.clear();

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

    // schedule DISCONNECT retransmission
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

    // schedule DISCONNECT retransmission
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

    // schedule PINGREQ retransmission
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

    // schedule DISCONNECT retransmission
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

std::vector<ClientState> MqttSNClient::getNextPossibleStates(ClientState currentState)
{
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
    // delete packet if the client is in DISCONNECTED or LOST state
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

    // delete packet if the client is ASLEEP and the message type is not DISCONNECT
    if (currentState == ClientState::ASLEEP && msgType != MsgType::DISCONNECT) {
        delete pk;
        return;
    }

    // delete packet if the client is AWAKE and the message type is not in the allowed list
    if (currentState == ClientState::AWAKE) {
        std::vector<MsgType> allowedMsgTypes = {MsgType::PINGRESP};
        adjustAllowedPacketTypes(allowedMsgTypes);

        if (std::find(allowedMsgTypes.begin(), allowedMsgTypes.end(), msgType) == allowedMsgTypes.end()) {
            delete pk;
            return;
        }
    }

    EV << "Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

    // validate received packet
    if (!isValidPacket(srcAddress, srcPort, msgType)) {
        delete pk;
        return;
    }

    // process packet based on the message type
    processPacketByMessageType(pk, srcAddress, srcPort, msgType);

    // additional custom packet processing
    processPacketCustom(pk, srcAddress, srcPort, msgType);

    // delete packet after processing
    delete pk;
}

void MqttSNClient::processPacketByMessageType(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
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
}

bool MqttSNClient::isValidPacket(const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    switch(msgType) {
        // packet types that are allowed only from the selected gateway
        case MsgType::CONNACK:
            if (!isSelectedGateway(srcAddress, srcPort)) {
                return false;
            }
            break;

        // packet types that are allowed only from the connected gateway
        case MsgType::PINGREQ:
        case MsgType::PINGRESP:
        case MsgType::DISCONNECT:
            if (!isConnectedGateway(srcAddress, srcPort)) {
                return false;
            }
            break;

        default:
            break;
    }

    return true;
}

void MqttSNClient::processAdvertise(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort)
{
    const auto& payload = pk->peekData<MqttSNAdvertise>();

    updateActiveGateways(srcAddress, srcPort, payload->getGwId(), payload->getDuration());
}

void MqttSNClient::processSearchGw()
{
    // no need for this client to send again SEARCHGW message
    if (searchGateway) {
        searchGateway = false;
    }

    if (!gateways.empty() && !gatewayInfoEvent->isScheduled()) {
        // delay sending of GWINFO message for a random time
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
        // GWINFO from other client
        inet::L3Address ipAddress = inet::L3AddressResolver().resolve(gatewayAddress.c_str());
        updateActiveGateways(ipAddress, (int) gatewayPort, gatewayId, 0);
    }
    else {
        // GWINFO from a server
        updateActiveGateways(srcAddress, srcPort, gatewayId, 0);

        // if client receives a GWINFO message, it will cancel the transmission of its GWINFO message
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

    processConnAckCustom();
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
        return;
    }

    EV << "Received ping response from server: " << srcAddress << ":" << srcPort << std::endl;
    unscheduleMsgRetransmission(MsgType::PINGREQ);
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
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

    MqttSNApp::socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
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
    MqttSNApp::corruptPacket(packet, MqttSNApp::packetBER);

    MqttSNApp::socket.sendTo(packet, destAddress, destPort);
}

void MqttSNClient::handleCheckGatewaysEvent()
{
    for (auto it = gateways.begin(); it != gateways.end();) {
        const GatewayInfo& gatewayInfo = it->second;

        // check if the elapsed time exceeds the threshold
        if ((getClockTime() - gatewayInfo.lastUpdatedTime) > ((int) par("nadv") * gatewayInfo.duration)) {
            // gateway is considered unavailable, remove it
            it = gateways.erase(it);
            continue;
        }

        ++it;
    }

    scheduleClockEventAfter(checkGatewaysInterval, checkGatewaysEvent);
}

void MqttSNClient::handleSearchGatewayEvent()
{
    // check if a search for gateways is needed
    if (!searchGateway) {
        return;
    }

    // send a SEARCHGW message
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
    if (gateways.empty()) {
        return;
    }

    std::pair<uint8_t, GatewayInfo> gateway = selectGateway();

    uint8_t gatewayId = gateway.first;
    GatewayInfo gatewayInfo = gateway.second;

    // client answers with a GWINFO message
    MqttSNApp::sendGwInfo(gatewayId, gatewayInfo.address.str(), gatewayInfo.port);
}

void MqttSNClient::handleCheckConnectionEvent()
{
    if (isConnected) {
        return;
    }

    // if there are active gateways, select one and handle the connection event
    if (!gateways.empty()) {
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

    // schedule PINGREQ retransmission
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, MsgType::PINGREQ);

    scheduleClockEventAfter(keepAlive, pingEvent);
}

void MqttSNClient::updateActiveGateways(const inet::L3Address& srcAddress, const int& srcPort, uint8_t gatewayId, uint16_t duration)
{
    // GWINFO messages use a temporary duration set in the client
    if (duration == 0) {
        duration = temporaryDuration;
    }

    auto it = gateways.find(gatewayId);
    if (it == gateways.end()) {
        GatewayInfo gatewayInfo;
        gatewayInfo.address = srcAddress;
        gatewayInfo.port = srcPort;
        gatewayInfo.duration = duration;
        gatewayInfo.lastUpdatedTime = getClockTime();

        gateways[gatewayId] = gatewayInfo;
        return;
    }

    // update the duration field only when an advertise message is received
    if (duration != temporaryDuration && it->second.duration != duration) {
        it->second.duration = duration;
    }

    it->second.lastUpdatedTime = getClockTime();
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

std::pair<uint8_t, GatewayInfo> MqttSNClient::selectGateway()
{
    if (gateways.empty()) {
        throw omnetpp::cRuntimeError("No active gateway found");
    }

    // random selection policy
    uint16_t index = intuniform(0, gateways.size() - 1);

    auto it = gateways.begin();
    std::advance(it, index);

    return std::make_pair(it->first, it->second);
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

void MqttSNClient::scheduleRetransmissionWithMsgId(MsgType msgType, uint16_t msgId)
{
    // schedule retransmission with only message ID parameter
    std::map<std::string, std::string> parameters;
    parameters["msgId"] = std::to_string(msgId);
    scheduleMsgRetransmission(selectedGateway.address, selectedGateway.port, msgType, &parameters);
}

bool MqttSNClient::checkMsgIdForType(MsgType msgType, uint16_t msgId)
{
    // check if the message type exists in the map
    auto it = retransmissions.find(msgType);
    if (it == retransmissions.end()) {
        // message type not found in retransmissions
        return false;
    }

    auto retransmissionEvent = it->second.retransmissionEvent;
    if (retransmissionEvent == nullptr) {
        // null retransmission event
        return false;
    }

    if (!retransmissionEvent->hasPar("msgId")) {
        // parameter not found in retransmission event
        return false;
    }

    // check if the stored message ID matches the input one
    return std::stoi(retransmissionEvent->par("msgId").stringValue()) == msgId;
}

bool MqttSNClient::processAckForMsgType(MsgType msgType, uint16_t msgId)
{
    if (!checkMsgIdForType(msgType, msgId)) {
        // either the ACK message ID does not match the expected message ID or the ACK message is already processed
        return false;
    }

    // ACK with correct message ID is received
    unscheduleMsgRetransmission(msgType);

    return true;
}

uint16_t MqttSNClient::getNewMsgId()
{
    return MqttSNApp::getNewIdentifier(getUsedMsgIds(), currentMsgId,
                                       "Failed to assign a new message ID. All available message IDs are in use");
}

std::set<uint16_t> MqttSNClient::getUsedMsgIds()
{
    std::set<uint16_t> usedIds;

    for (const auto& entry : retransmissions) {
        if (entry.second.retransmissionEvent->hasPar("msgId")) {
            // extract and insert message ID parameter value into the set
            usedIds.insert(std::stoi(entry.second.retransmissionEvent->par("msgId").stringValue()));
        }
    }

    return usedIds;
}

void MqttSNClient::checkTopicConsistency(const std::string& topicName, TopicIdType topicIdType, bool isFound)
{
    if (topicIdType == TopicIdType::PRE_DEFINED_TOPIC_ID) {
        // topic name must be present in the predefined topics definition
        if (!isFound) {
            throw omnetpp::cRuntimeError("Predefined topic '%s' is not defined", topicName.c_str());
        }
        return;
    }

    // topic name should not be present in the predefined topics definition
    if (isFound) {
        throw omnetpp::cRuntimeError("Topic '%s' has an ambiguous type", topicName.c_str());
    }
}

uint16_t MqttSNClient::getPredefinedTopicId(const std::string& topicName)
{
    // check if the predefined topic exists
    auto predefinedTopicsIt = predefinedTopics.find(StringHelper::base64Encode(topicName));
    if (predefinedTopicsIt == predefinedTopics.end()) {
        throw omnetpp::cRuntimeError("Predefined topic '%s' is not defined", topicName.c_str());
    }

    return predefinedTopicsIt->second;
}

void MqttSNClient::handleFinalSimulationResults()
{
    static bool resultsProcessed = false;

    if (!resultsProcessed) {
        std::cout << "==== Publish Messages Results ====" << std::endl;

        // compute and print the results
        printStatistics();
        computePublishEndToEndDelay();
        computePublishHitRate();

        // save results
        appendSimulationResultsToCsv("results/results.csv");

        resultsProcessed = true;
    }
}

void MqttSNClient::printStatistics()
{
    std::cout << "Unique sent: " << sentUniquePublishMsgs << std::endl;
    std::cout << "Unique received: " << receivedUniquePublishMsgs << std::endl;
    std::cout << "Total received: " << receivedTotalPublishMsgs << std::endl;
    std::cout << "Total received duplicates: " << receivedDuplicatePublishMsgs << std::endl;
    std::cout << std::endl;
}

void MqttSNClient::computePublishEndToEndDelay()
{
    if (receivedTotalPublishMsgs > 0) {
        std::cout << "Average end-to-end delay: " << sumReceivedPublishMsgTimestamps / receivedTotalPublishMsgs << " seconds" << std::endl;
        return;
    }

    std::cout << "No publish messages received to calculate average delay" << std::endl;
}

void MqttSNClient::computePublishHitRate()
{
    if (sentUniquePublishMsgs > 0) {
        std::cout << "Hit rate: " << static_cast<double>(receivedUniquePublishMsgs) / sentUniquePublishMsgs * 100 << " %" << std::endl;
        return;
    }

    std::cout << "No publish messages sent to calculate hit rate" << std::endl;
}

void MqttSNClient::appendSimulationResultsToCsv(const std::string& filePath)
{
    // check if the CSV file already exists
    std::ifstream infile(filePath);
    bool fileExists = infile.good();

    // open the CSV file in append mode
    std::ofstream outfile(filePath, std::ios::app);

    // if the file does not exist, write the column headers
    if (!fileExists) {
        outfile << "BER,Average End-to-End Delay,Hit Rate,Total Received Duplicates,"
                   "Publishers Retransmissions,Servers Retransmissions,Subscribers Retransmissions\n";
    }

    // calculate average end-to-end delay
    double averageDelay = receivedTotalPublishMsgs > 0 ? sumReceivedPublishMsgTimestamps / receivedTotalPublishMsgs : 0;

    // calculate hit rate
    double hitRate = sentUniquePublishMsgs > 0 ? static_cast<double>(receivedUniquePublishMsgs) / sentUniquePublishMsgs * 100 : 0;

    // write the simulation results to the file
    outfile << MqttSNApp::packetBER << "," << averageDelay << "," << hitRate << "," << receivedDuplicatePublishMsgs << ","
            << publishersRetransmissions << "," << MqttSNApp::serversRetransmissions << "," <<subscribersRetransmissions << "\n";

    // close the files
    infile.close();
    outfile.close();
}

void MqttSNClient::scheduleMsgRetransmission(const inet::L3Address& destAddress, const int& destPort, MsgType msgType,
                                             std::map<std::string, std::string>* parameters)
{
    // check if a message of the same type is already scheduled for retransmission
    if (retransmissions.find(msgType) != retransmissions.end()) {
        // exit without doing anything
        return;
    }

    // create a new structure for this message type
    RetransmissionInfo retransmissionInfo;
    retransmissionInfo.retransmissionEvent = new inet::ClockEvent("retransmissionTimer");
    retransmissionInfo.retransmissionCounter = 0;
    retransmissionInfo.destAddress = destAddress;
    retransmissionInfo.destPort = destPort;

    // add other dynamic parameters to the timer
    if (parameters != nullptr) {
        for (const auto& param : *parameters) {
            retransmissionInfo.retransmissionEvent->addPar(param.first.c_str()).setStringValue(param.second.c_str());
        }
    }

    // flag to identify this event as a retransmission message
    retransmissionInfo.retransmissionEvent->addPar("isRetransmissionEvent");
    // set the message type as a parameter
    retransmissionInfo.retransmissionEvent->addPar("messageType").setLongValue(static_cast<long>(msgType));

    // add the timer and information to the retransmissions map
    retransmissions[msgType] = retransmissionInfo;

    // start the timer
    scheduleClockEventAfter(MqttSNApp::retransmissionInterval, retransmissionInfo.retransmissionEvent);
}

void MqttSNClient::unscheduleMsgRetransmission(MsgType msgType)
{
    // find the element in the map with the specified message type
    auto it = retransmissions.find(msgType);
    if (it != retransmissions.end()) {
        RetransmissionInfo& retransmissionInfo = it->second;
        // cancel the event inside the struct
        cancelAndDelete(retransmissionInfo.retransmissionEvent);

        // remove the element from the map
        retransmissions.erase(it);
    }
}

void MqttSNClient::clearRetransmissions()
{
    // clear the map to remove all elements
    for (auto it = retransmissions.begin(); it != retransmissions.end();) {
        // cancel all associated events in the map
        cancelAndDelete(it->second.retransmissionEvent);

        // remove the element from the map
        it = retransmissions.erase(it);
    }
}

void MqttSNClient::handleRetransmissionEvent(omnetpp::cMessage* msg)
{
    // get the message type from the message parameter
    MsgType msgType = static_cast<MsgType>(msg->par("messageType").longValue());

    // check the message type in the map
    auto it = retransmissions.find(msgType);
    if (it == retransmissions.end()) {
        // if not found, exit the function
        return;
    }

    RetransmissionInfo* retransmissionInfo = &it->second;

    // check if the number of retries equals the threshold
    if (retransmissionInfo->retransmissionCounter >= MqttSNApp::retransmissionCounter) {
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
            retransmitDisconnect(retransmissionInfo->destAddress, retransmissionInfo->destPort, msg);
            break;

        case MsgType::PINGREQ:
            retransmitPingReq(retransmissionInfo->destAddress, retransmissionInfo->destPort, msg);
            break;

        default:
            break;
    }

    handleRetransmissionEventCustom(retransmissionInfo->destAddress, retransmissionInfo->destPort, msg, msgType);

    retransmissionInfo->retransmissionCounter++;
    scheduleClockEventAfter(MqttSNApp::retransmissionInterval, retransmissionInfo->retransmissionEvent);
}

void MqttSNClient::retransmitDisconnect(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    if (msg->hasPar("sleepDuration")) {
        MqttSNApp::sendDisconnect(destAddress, destPort, std::stoul(msg->par("sleepDuration").stringValue()));
    }
    else {
        MqttSNApp::sendDisconnect(destAddress, destPort);
    }

    updateRetransmissionsCounter();
}

void MqttSNClient::retransmitPingReq(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg)
{
    if (msg->hasPar("clientId")) {
        MqttSNApp::sendPingReq(destAddress, destPort, msg->par("clientId").stringValue());
    }
    else {
        MqttSNApp::sendPingReq(destAddress, destPort);
    }

    updateRetransmissionsCounter();
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
