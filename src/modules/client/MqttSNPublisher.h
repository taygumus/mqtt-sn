#ifndef MODULES_CLIENT_MQTTSNPUBLISHER_H_
#define MODULES_CLIENT_MQTTSNPUBLISHER_H_

#include "MqttSNClient.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/client/publisher/DataInfo.h"
#include "types/client/publisher/TopicAndData.h"
#include "types/client/publisher/RegisterInfo.h"
#include "types/client/publisher/LastRegisterInfo.h"
#include "types/client/publisher/LastPublishInfo.h"

namespace mqttsn {

class MqttSNPublisher : public MqttSNClient
{
    protected:
        // parameters
        int willQosFlag;
        bool willRetainFlag;
        std::string willTopic;
        std::string willMsg;
        double registrationInterval;
        double publishInterval;
        double waitingInterval;

        // active publisher state
        std::map<int, TopicAndData> topicsAndData;

        inet::ClockEvent* registrationEvent = nullptr;
        std::map<uint16_t, RegisterInfo> topicIds;
        LastRegisterInfo lastRegistration;

        inet::ClockEvent* publishEvent = nullptr;
        LastPublishInfo lastPublish;

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
        virtual void processWillTopicReq(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillMsgReq(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillResp(inet::Packet* pk, bool willTopic);
        virtual void processRegAck(inet::Packet* pk);
        virtual void processPubAck(inet::Packet* pk);
        virtual void processPubRec(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubComp(inet::Packet* pk);

        // send packets
        virtual void sendBaseWithWillTopic(const inet::L3Address& destAddress, const int& destPort,
                                           MsgType msgType,
                                           QoS qosFlag, bool retainFlag,
                                           const std::string& willTopic);

        virtual void sendBaseWithWillMsg(const inet::L3Address& destAddress, const int& destPort,
                                         MsgType msgType,
                                         const std::string& willMsg);

        virtual void sendRegister(const inet::L3Address& destAddress, const int& destPort,
                                  uint16_t msgId,
                                  const std::string& topicName);

        virtual void sendPublish(const inet::L3Address& destAddress, const int& destPort,
                                 bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag,
                                 uint16_t topicId, uint16_t msgId,
                                 const std::string& data);

        virtual void sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId);

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;
        virtual void handleRegistrationEvent();
        virtual void handlePublishEvent();

        // other methods
        virtual void fillTopicsAndData();
        virtual void retryLastPublish();
        virtual bool findTopicByName(const std::string& topicName, uint16_t& topicId);

        // retransmissions management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                     omnetpp::cMessage* msg, MsgType msgType) override;

        virtual void retransmitWillTopicUpd(const inet::L3Address& destAddress, const int& destPort);
        virtual void retransmitWillMsgUpd(const inet::L3Address& destAddress, const int& destPort);
        virtual void retransmitRegister(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitPublish(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitPubRel(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);

    public:
        MqttSNPublisher() {};
        ~MqttSNPublisher();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNPUBLISHER_H_ */
