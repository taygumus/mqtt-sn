#ifndef MODULES_CLIENT_MQTTSNSUBSCRIBER_H_
#define MODULES_CLIENT_MQTTSNSUBSCRIBER_H_

#include "MqttSNClient.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/shared/ReturnCode.h"
#include "types/shared/TagInfo.h"
#include "types/client/subscriber/ItemInfo.h"
#include "types/client/subscriber/TopicInfo.h"
#include "types/client/subscriber/LastOperationInfo.h"
#include "types/client/subscriber/DataInfo.h"
#include "types/client/subscriber/MessageInfo.h"

namespace mqttsn {

class MqttSNSubscriber : public MqttSNClient
{
    protected:
        // parameters
        double subscriptionInterval;
        double unsubscriptionInterval;

        // active subscriber state
        std::map<int, ItemInfo> items;

        inet::ClockEvent* subscriptionEvent = nullptr;
        LastOperationInfo lastSubscription;
        int subscriptionCounter = 0;

        std::map<uint16_t, TopicInfo> topics;

        inet::ClockEvent* unsubscriptionEvent = nullptr;
        LastOperationInfo lastUnsubscription;
        int unsubscriptionCounter = 0;

        std::map<uint16_t, DataInfo> messages;

        // metrics attributes
        static std::set<unsigned> publishMsgIdentifiers;

    protected:
        // initialization
        virtual void levelTwoInit() override;

        // message handling
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage* msg) override;

        // active state management
        virtual void scheduleActiveStateEventsCustom() override;
        virtual void cancelActiveStateEventsCustom() override;
        virtual void cancelActiveStateClockEventsCustom() override;

        // incoming packet handling
        virtual void adjustAllowedPacketTypes(std::vector<MsgType>& msgTypes) override;
        virtual void processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType) override;
        virtual void processConnAckCustom() override;

        // incoming packet type methods
        virtual void processSubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processUnsubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processRegister(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPublish(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubRel(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);

        // outgoing packet handling
        virtual void sendSubscribe(const inet::L3Address& destAddress, const int& destPort, bool dupFlag, QoS qosFlag,
                                   TopicIdType topicIdTypeFlag, uint16_t msgId, const std::string& topicName, uint16_t topicId,
                                   bool useTopicId = false);

        virtual void sendUnsubscribe(const inet::L3Address& destAddress, const int& destPort, TopicIdType topicIdTypeFlag, uint16_t msgId,
                                     const std::string& topicName, uint16_t topicId, bool useTopicId = false);

        virtual void sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t topicId,
                                              uint16_t msgId, ReturnCode returnCode);

        virtual void sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId);

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;
        virtual void handleSubscriptionEvent();
        virtual void handleUnsubscriptionEvent();

        // item methods
        virtual void populateItems() override;
        virtual ItemInfo* findItemByTopicName(const std::string& topicName);

        // topic methods
        virtual void resetAndPopulateTopics();
        virtual void addNewTopic(uint16_t topicId, const std::string& topicName, ItemInfo* itemInfo);
        virtual void validateTopic(const std::string& topicName, uint16_t topicId, bool useTopicId);
        virtual bool proceedWithSubscription();
        virtual bool proceedWithUnsubscription();

        // publication methods
        virtual void printPublishMessage(const MessageInfo& messageInfo);
        virtual void handlePublishMessageMetrics(const TagInfo& tagInfo);

        // retransmission management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg,
                                                     MsgType msgType) override;

        virtual void retransmitSubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitUnsubscribe(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);

    public:
        MqttSNSubscriber() {};
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
