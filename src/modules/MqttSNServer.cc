#include "MqttSNServer.h"

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

void MqttSNServer::processStart()
{
    //
}

void MqttSNServer::processSend()
{
    //sendPacket();
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
    EV_INFO << "Received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;
    delete pk;
}

MqttSNServer::~MqttSNServer()
{
    cancelAndDelete(advertiseMsg);
}

} /* namespace mqttsn */
