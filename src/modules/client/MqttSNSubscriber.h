#ifndef MODULES_CLIENT_MQTTSNSUBSCRIBER_H_
#define MODULES_CLIENT_MQTTSNSUBSCRIBER_H_

#include "MqttSNClient.h"
#include "types/shared/QoS.h"
#include "types/client/subscriber/Topic.h"

namespace mqttsn {

class MqttSNSubscriber : public MqttSNClient
{
    protected:
        // parameters
        double subscriptionInterval;

        // active subscriber state
        std::map<int, Topic> topics;

        inet::ClockEvent* subscriptionEvent = nullptr;

    protected:
        virtual void initializeCustom() override;
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage* msg) override;

        // active state management
        virtual void scheduleActiveStateEventsCustom() override;
        virtual void cancelActiveStateEventsCustom() override;
        virtual void cancelActiveStateClockEventsCustom() override;

        // process received packets
        virtual void processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType) override;
        virtual void processConnAckCustom() override;

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;
        virtual void handleSubscriptionEvent();

        // other methods
        virtual void fillTopics();

        // retransmissions management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                     omnetpp::cMessage* msg, MsgType msgType) override;

    public:
        MqttSNSubscriber() {};
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
