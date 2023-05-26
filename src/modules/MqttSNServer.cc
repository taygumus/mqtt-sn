#include "MqttSNServer.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "messages/MqttSNAdvertise.h"

namespace mqttsn {

Define_Module(MqttSNServer);

void MqttSNServer::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {

        numAdvertiseSent = 0;
        WATCH(numAdvertiseSent);

        startTime = par("startTime");
        stopTime = par("stopTime");

        if (stopTime >= inet::CLOCKTIME_ZERO && stopTime < startTime)
            throw omnetpp::cRuntimeError("Invalid startTime/stopTime parameters");

        advertiseEvent = new inet::ClockEvent("advertiseTimer");
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg == advertiseEvent) {
        sendPacket();
        inet::clocktime_t d = par("advertiseInterval");
        if (stopTime < inet::CLOCKTIME_ZERO || getClockTime() + d < stopTime) {
            scheduleClockEventAfter(d, advertiseEvent);
        }
    }
    else
        socket.processMessage(msg);
}

void MqttSNServer::handleStartOperation(inet::LifecycleOperation *operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    inet::clocktime_t start = std::max(startTime, getClockTime());
    if ((stopTime < inet::CLOCKTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
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
    EV_INFO << "...Received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;
    delete pk;
}

void MqttSNServer::sendPacket()
{
    const auto& payload = inet::makeShared<MqttSNAdvertise>();
    payload->setMsgType(MsgType::ADVERTISE);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::ostringstream str;
    str << "AdvertisePacket"<< "-" << numAdvertiseSent;
    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address("255.255.255.255"), par("destPort"));
    numAdvertiseSent++;
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseEvent);
}

} /* namespace mqttsn */
