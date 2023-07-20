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

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
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

        willFlag = par("willFlag");
    }
}

void MqttSNClient::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == checkGatewaysEvent) {
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

    scheduleClockEventAt(checkGatewaysInterval, checkGatewaysEvent);
    scheduleClockEventAt(searchGatewayInterval, searchGatewayEvent);
    scheduleClockEventAt(checkConnectionInterval, checkConnectionEvent);
}

void MqttSNClient::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(checkGatewaysEvent);
    cancelEvent(searchGatewayEvent);
    cancelEvent(gatewayInfoEvent);
    cancelEvent(checkConnectionEvent);

    socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(checkGatewaysEvent);
    cancelClockEvent(searchGatewayEvent);
    cancelClockEvent(gatewayInfoEvent);
    cancelClockEvent(checkConnectionEvent);

    socket.destroy();
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
            processConnAck(pk, srcAddress, srcPort);
            break;

        default:
            throw omnetpp::cRuntimeError("Unknown message type: %d", (uint16_t) header->getMsgType());
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

void MqttSNClient::processConnAck(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNBaseWithReturnCode>();
    ReturnCode returnCode = payload->getReturnCode();

    // TO DO -> manage the other codes
    if (returnCode != ReturnCode::ACCEPTED) {
        return;
    }

    // client is connected
    isConnected = true;
    connectedGateway.address = srcAddress;
    connectedGateway.port = srcPort;
    connectedGateway.duration = 0;
    connectedGateway.lastUpdatedTime = getClockTime();

    std::ostringstream str;
    str << "Client connected to: " << srcAddress.str();
    bubble(str.str().c_str());
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
    payload->setClientId(clientId);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet *packet = new inet::Packet("ConnectPacket");
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
    // selection policy -> first element
    auto firstElement = *activeGateways.begin();

    uint8_t gatewayId = firstElement.first;
    GatewayInfo gatewayInfo = firstElement.second;

    // client answers with a gwInfo message
    sendGwInfo(gatewayId, gatewayInfo.address.str(), gatewayInfo.port);
}

void MqttSNClient::handleCheckConnectionEvent()
{
    if (!activeGateways.empty() && !isConnected) {
        // selection policy -> first element
        // TO DO -> change policy otherwise every client selects that server
        auto firstElement = *activeGateways.begin();
        GatewayInfo gatewayInfo = firstElement.second;

        // TO DO -> keep-alive
        sendConnect(willFlag, false, 0, gatewayInfo.address, gatewayInfo.port);
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

std::string MqttSNClient::generateClientId()
{
    uint16_t length = intuniform(Length::ONE_OCTET, Length::CLIENT_ID_OCTETS);

    std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string clientId;

    for (uint16_t i = 0; i < length; i++) {
        clientId += allowedChars[intuniform(0, allowedChars.length() - 1)];
    }

    return clientId;
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
    cancelAndDelete(gatewayInfoEvent);
    cancelAndDelete(checkConnectionEvent);
}

} /* namespace mqttsn */
