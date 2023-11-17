#ifndef MODULES_CLIENT_MQTTSNSUBSCRIBER_H_
#define MODULES_CLIENT_MQTTSNSUBSCRIBER_H_

#include "MqttSNClient.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/client/subscriber/Topic.h"
#include "types/client/subscriber/TopicInfo.h"
#include "types/client/subscriber/LastSubscriptionInfo.h"

namespace mqttsn {

class MqttSNSubscriber : public MqttSNClient
{
    protected:
        // parameters
        double subscriptionInterval;
        double unsubscriptionInterval;

        // active subscriber state
        std::map<int, Topic> topics;

        inet::ClockEvent* subscriptionEvent = nullptr;
        std::map<uint16_t, TopicInfo> topicIds;
        LastSubscriptionInfo lastSubscription;

        inet::ClockEvent* unsubscriptionEvent = nullptr;

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
        virtual void processSubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);

        // send packets
        virtual void sendSubscribe(const inet::L3Address& destAddress, const int& destPort,
                                   bool dupFlag, QoS qosFlag, TopicIdType topicIdTypeFlag,
                                   uint16_t msgId,
                                   const std::string& topicName, uint16_t topicId);

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;
        virtual void handleSubscriptionEvent();
        virtual void handleUnsubscriptionEvent();

        // other methods
        virtual void fillTopics();

        // retransmissions management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                     omnetpp::cMessage* msg, MsgType msgType) override;

        virtual void retransmitSubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);

    public:
        MqttSNSubscriber() {};
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
