#ifndef MODULES_CLIENT_MQTTSNPUBLISHER_H_
#define MODULES_CLIENT_MQTTSNPUBLISHER_H_

#include "MqttSNClient.h"

namespace mqttsn {

class MqttSNPublisher : public MqttSNClient
{
    protected:
        virtual void initializeCustom() override;
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage *msg) override;

        // client state management
        virtual void scheduleActiveStateEventsCustom() override;
        virtual void cancelActiveStateEventsCustom() override;
        virtual void cancelActiveStateClockEventsCustom() override;

        // process received packets
        virtual void processPacketCustom(MsgType msgType, inet::Packet *pk, inet::L3Address srcAddress, int srcPort) override;
        virtual void processWillTopicReq(inet::L3Address srcAddress, int srcPort);
        virtual void processWillMsgReq(inet::L3Address srcAddress, int srcPort);

        // send packets
        virtual void sendBaseWithWillTopic(inet::L3Address destAddress, int destPort, MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic);
        virtual void sendBaseWithWillMsg(inet::L3Address destAddress, int destPort, MsgType msgType, std::string willMsg);

        // event handlers
        virtual void handleCheckConnectionEventCustom(inet::L3Address destAddress, int destPort) override;

        // retransmissions management
        virtual void handleRetransmissionEventCustom(MsgType msgType, inet::L3Address destAddress, int destPort, omnetpp::cMessage *msg, bool retransmission = true) override;

    public:
        MqttSNPublisher() {};
        ~MqttSNPublisher();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNPUBLISHER_H_ */
