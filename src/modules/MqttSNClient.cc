#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "types/MsgType.h"
#include "messages/MqttSNBase.h"
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

    const auto& header = pk->peekData<MqttSNBase>();
    checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());

    switch(header->getMsgType()) {
        case MsgType::ADVERTISE:
            processAdvertise(pk);
            break;
        case MsgType::SEARCHGW:
            processSearchGateway(pk);
            break;

        default:
            throw omnetpp::cRuntimeError("Unknown message type: %d", (uint16_t) header->getMsgType());
    }

    delete pk;
}

void MqttSNClient::processAdvertise(inet::Packet *pk)
{
    const auto& payload = pk->peekData<MqttSNAdvertise>();

    inet::L3Address srcAddress = pk->getTag<inet::L3AddressInd>()->getSrcAddress();
    int srcPort = pk->getTag<inet::L4PortInd>()->getSrcPort();

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

void MqttSNClient::processSearchGateway(inet::Packet *pk)
{
    // no need for this client to send again the search gateway message
    searchGateway = false;
}

void MqttSNClient::sendSearchGateway()
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
        searchGatewayInterval *= searchGatewayInterval;
        double maxInterval = par("maxSearchGatewayInterval");

        if (searchGatewayInterval > maxInterval) {
            searchGatewayInterval = maxInterval;
            maxIntervalReached = true;
        }
    }

    scheduleClockEventAfter(searchGatewayInterval, searchGatewayEvent);
}

void MqttSNClient::handleSearchGatewayEvent()
{
    if (searchGateway) {
        sendSearchGateway();
        handleSearchGatewayInterval();
    }
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
}

} /* namespace mqttsn */
