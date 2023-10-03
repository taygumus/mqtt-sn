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
        virtual void processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType) override;

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;

        // retransmissions management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg, MsgType msgType, bool retransmission = true) override;

    public:
        MqttSNSubscriber() {};
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
