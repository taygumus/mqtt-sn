#ifndef MODULES_CLIENT_MQTTSNSUBSCRIBER_H_
#define MODULES_CLIENT_MQTTSNSUBSCRIBER_H_

#include "MqttSNClient.h"

namespace mqttsn {

class MqttSNSubscriber : public MqttSNClient
{
    protected:
        virtual void initializeCustom() override;
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage* msg) override;

        // client state management
        virtual void scheduleActiveStateEventsCustom() override;
        virtual void cancelActiveStateEventsCustom() override;
        virtual void cancelActiveStateClockEventsCustom() override;

        // process received packets
        virtual void processPacketCustom(MsgType msgType, inet::Packet* pk, inet::L3Address srcAddress, int srcPort) override;

        // event handlers
        virtual void handleCheckConnectionEventCustom(inet::L3Address destAddress, int destPort) override;

        // retransmissions management
        virtual void handleRetransmissionEventCustom(MsgType msgType, inet::L3Address destAddress, int destPort, omnetpp::cMessage* msg, bool retransmission = true) override;

    public:
        MqttSNSubscriber() {};
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
