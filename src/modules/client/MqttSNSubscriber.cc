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

void MqttSNSubscriber::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    // TO DO
}

void MqttSNSubscriber::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSessionFlag"), keepAlive);
}

void MqttSNSubscriber::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg, MsgType msgType, bool retransmission)
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
