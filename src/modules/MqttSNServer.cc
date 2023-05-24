#include "MqttSNServer.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "messages/MqttSNAdvertise.h"

namespace mqttsn {

Define_Module(MqttSNServer);

void MqttSNServer::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {

        startTime = par("startTime");
        stopTime = par("stopTime");

        if (stopTime >= inet::CLOCKTIME_ZERO && stopTime < startTime)
            throw omnetpp::cRuntimeError("Invalid startTime/stopTime parameters");

        advertiseMsg = new inet::ClockEvent("advertiseTimer");
    }
}

void MqttSNServer::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg->isSelfMessage()) {

        if (msg == advertiseMsg ) {
            EV << "Entratooooooooo" << std::endl;
        }

        switch (advertiseMsg->getKind()) {
            case START:
                processStart();
                break;

            case SEND:
                processSend();
                break;

            case STOP:
                processStop();
                break;

            default:
                throw omnetpp::cRuntimeError("Invalid kind %d in self message", (int)advertiseMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void MqttSNServer::handleStartOperation(inet::LifecycleOperation *operation)
{
    inet::clocktime_t start = std::max(startTime, getClockTime());
    if ((stopTime < inet::CLOCKTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        advertiseMsg->setKind(START);
        scheduleClockEventAt(start, advertiseMsg);
    }
}

void MqttSNServer::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(advertiseMsg);
    socket.close();
}

void MqttSNServer::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(advertiseMsg);
    socket.destroy();
}

void MqttSNServer::processPacket(inet::Packet *pk)
{
    EV_INFO << "...Received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;
    delete pk;
}

void MqttSNServer::processStart()
{
    const char *localAddress = par("localAddress");

    socket.setOutputGate(gate("socketOut"));
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), 2000);
    socket.setBroadcast(true);
    socket.setCallback(this);

    advertiseMsg->setKind(SEND);
    processSend();
}

void MqttSNServer::processSend()
{
    sendPacket();
    inet::clocktime_t d = par("advertiseInterval");
    if (stopTime < inet::CLOCKTIME_ZERO || getClockTime() + d < stopTime) {
        advertiseMsg->setKind(SEND);
        scheduleClockEventAfter(d, advertiseMsg);
    }
    else {
        advertiseMsg->setKind(STOP);
        scheduleClockEventAt(stopTime, advertiseMsg);
    }
}

void MqttSNServer::processStop()
{
    socket.close();
}

void MqttSNServer::sendPacket()
{
    EV << "Client is sending a new packet..\n";

    const auto& payload = inet::makeShared<MqttSNAdvertise>();
    payload->setMsgType(MsgType::ADVERTISE);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::ostringstream str;
    str << "Advertise Packet";

    inet::Packet *packet = new inet::Packet(str.str().c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, inet::L3Address("255.255.255.255"), 2000);
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseMsg);
}

} /* namespace mqttsn */
