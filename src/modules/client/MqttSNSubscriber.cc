#include "MqttSNSubscriber.h"

namespace mqttsn {

Define_Module(MqttSNSubscriber);

void MqttSNSubscriber::initializeCustom()
{
    // TO DO
}

bool MqttSNSubscriber::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    // TO DO
    return false;
}

void MqttSNSubscriber::scheduleActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNSubscriber::cancelActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNSubscriber::processPacketCustom(MsgType msgType, inet::Packet* pk, inet::L3Address srcAddress, int srcPort)
{
    // TO DO
}

void MqttSNSubscriber::handleCheckConnectionEventCustom(inet::L3Address destAddress, int destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSessionFlag"), keepAlive);
}

void MqttSNSubscriber::handleRetransmissionEventCustom(MsgType msgType, inet::L3Address destAddress, int destPort, omnetpp::cMessage* msg, bool retransmission)
{
    // TO DO
    switch (msgType) {
        //
        default:
            break;
    }
}

void MqttSNSubscriber::cancelActiveStateClockEventsCustom()
{
    // TO DO
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    // TO DO
}

} /* namespace mqttsn */
