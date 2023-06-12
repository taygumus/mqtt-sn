#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "types/MsgType.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNSearchGw.h"

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
    }
}

void MqttSNClient::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == checkGatewaysEvent) {
        checkGatewaysAvailability();
        scheduleClockEventAfter(checkGatewaysInterval, checkGatewaysEvent);
    }
    else if(msg == searchGatewayEvent) {
        handleSearchGatewayEvent();
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
}

void MqttSNClient::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(checkGatewaysEvent);
    cancelEvent(searchGatewayEvent);
    socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(checkGatewaysEvent);
    cancelClockEvent(searchGatewayEvent);
    socket.destroy();
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    EV << "Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();
    if (isSelfBroadcastAddress(srcAddress)) {
        delete pk;
        return;
    }

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
            // TO DO
            // block the gwInfo to send in processSearchGw AND
            // process gwInfo updating the activeGatways
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

    auto it = activeGateways.find(gatewayId);

    if (it == activeGateways.end()) {
        // gatewayId not found in the map
        GatewayInfo gatewayInfo;
        gatewayInfo.address = srcAddress;
        gatewayInfo.port = srcPort;
        gatewayInfo.duration = duration;
        gatewayInfo.lastAdvertiseTime = getClockTime();

        activeGateways[gatewayId] = gatewayInfo;
    }
    else {
        it->second.lastAdvertiseTime = getClockTime();
    }
}

void MqttSNClient::processSearchGw(inet::Packet *pk)
{
    // no need for this client to send again the search gateway message
    searchGateway = false;

    // if I have any gateway in the list I'll send a GwInfo message in broadcast (to manage the priority)
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

void MqttSNClient::checkGatewaysAvailability()
{
    int nadv = par("nadv");
    inet::clocktime_t currentTime = getClockTime();

    for (auto it = activeGateways.begin(); it != activeGateways.end();) {

        const GatewayInfo& gatewayInfo = it->second;
        inet::clocktime_t elapsedTime = currentTime - gatewayInfo.lastAdvertiseTime;

        if (elapsedTime >= (nadv * gatewayInfo.duration)) {
            // gateway is considered unavailable
            it = activeGateways.erase(it);
        }
        else {
            ++it;
        }
    }
}

void MqttSNClient::handleSearchGatewayInterval()
{
    if (!maxIntervalReached) {
        double maxInterval = par("maxSearchGatewayInterval");

        searchGatewayInterval = std::min(searchGatewayInterval * searchGatewayInterval, maxInterval);
        maxIntervalReached = (searchGatewayInterval == maxInterval);
    }

    scheduleClockEventAfter(searchGatewayInterval, searchGatewayEvent);
}

void MqttSNClient::handleSearchGatewayEvent()
{
    if (searchGateway) {
        sendSearchGw();
        handleSearchGatewayInterval();
    }
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
}

} /* namespace mqttsn */
