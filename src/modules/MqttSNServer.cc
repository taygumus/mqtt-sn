#include "MqttSNServer.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "messages/MqttSNAdvertise.h"

namespace mqttsn {

Define_Module(MqttSNServer);

int MqttSNServer::gatewayIdCounter = -1;

void MqttSNServer::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        startAdvertise = par("startAdvertise");
        stopAdvertise = par("stopAdvertise");

        if (stopAdvertise >= inet::CLOCKTIME_ZERO && stopAdvertise < startAdvertise)
            throw omnetpp::cRuntimeError("Invalid startAdvertise/stopAdvertise parameters");
        advertiseEvent = new inet::ClockEvent("advertiseTimer");

        if (gatewayIdCounter < UINT8_MAX)
            gatewayIdCounter++;
        else
            throw omnetpp::cRuntimeError("The gateway ID counter has reached its maximum limit");
        gatewayId = gatewayIdCounter;
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == advertiseEvent) {
        sendPacket();
        inet::clocktime_t d = par("advertiseInterval");
        if (stopAdvertise < inet::CLOCKTIME_ZERO || getClockTime() + d < stopAdvertise) {
            scheduleClockEventAfter(d, advertiseEvent);
        }
    }
    else
        socket.processMessage(msg);
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
    EV_INFO << "...Server received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;
    delete pk;
}

void MqttSNServer::sendPacket()
{
    const auto& payload = inet::makeShared<MqttSNAdvertise>();
    payload->setMsgType(MsgType::ADVERTISE);
    payload->setGwId(gatewayId);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::ostringstream str;
    str << "AdvertisePacket"<< "-" << numAdvertiseSent;
    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address(par("broadcastAddress")), par("destPort"));
    numAdvertiseSent++;
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseEvent);
}

} /* namespace mqttsn */
