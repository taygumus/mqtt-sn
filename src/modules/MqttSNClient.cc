#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "types/MsgType.h"
#include "messages/MqttSNAdvertise.h"
#include "messages/MqttSNSearchGw.h"
#include "messages/MqttSNGwInfo.h"

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

    // Print elements
        for (const auto& entry : activeGateways) {
            uint8_t gatewayId = entry.first;
            const GatewayInfo& gatewayInfo = entry.second;

            EV << "Gateway ID: " << static_cast<int>(gatewayId) << std::endl;
            EV << "Address: " << gatewayInfo.address.str() << std::endl;
            EV << "Port: " << gatewayInfo.port << std::endl;
            EV << "Duration: " << gatewayInfo.duration << std::endl;
            EV << "Last Updated Time: " << gatewayInfo.lastUpdatedTime << std::endl;
            EV << std::endl;
        }
        //
}

void MqttSNClient::processSearchGw(inet::Packet *pk)
{
    // no need for this client to send again the search gateway message
    if (searchGateway) {
        searchGateway = false;
    }

    // TO DO
    // if I have any gateway in the list I'll SEND a GwInfo message in broadcast (to manage the priority)
}

void MqttSNClient::processGwInfo(inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    const auto& payload = pk->peekData<MqttSNGwInfo>();

    uint8_t gatewayId = payload->getGwId();
    //uint16_t gatewayAddress = payload->getGwAdd();
// risposta da client o server (da gestire)

    updateActiveGateways(gatewayId, 0, srcAddress, srcPort);

    if (searchGateway) {
        searchGateway = false;
    }
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
        // we update the duration field only when we receive an advertise message
        if (duration != temporaryDuration && it->second.duration != duration) {
            it->second.duration = duration;
        }

        it->second.lastUpdatedTime = getClockTime();
    }
}

MqttSNClient::~MqttSNClient()
{
    cancelAndDelete(checkGatewaysEvent);
    cancelAndDelete(searchGatewayEvent);
}

} /* namespace mqttsn */
