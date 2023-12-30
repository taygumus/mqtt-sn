#ifndef MODULES_SERVER_MQTTSNSERVER_H_
#define MODULES_SERVER_MQTTSNSERVER_H_

#include "../MqttSNApp.h"
#include "types/shared/MsgType.h"
#include "types/shared/ReturnCode.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/shared/ClientState.h"
#include "types/server/GatewayState.h"
#include "types/server/ClientType.h"
#include "types/server/ClientInfo.h"
#include "types/server/DataInfo.h"
#include "types/server/PublisherInfo.h"
#include "types/server/TopicInfo.h"
#include "types/server/RetainMessageInfo.h"
#include "types/server/MessageInfo.h"
#include "types/server/RequestInfo.h"
#include "types/server/RegisterInfo.h"
#include "types/server/SubscriberInfo.h"

namespace mqttsn {

class MqttSNServer : public MqttSNApp
{
    protected:
        // parameters
        uint16_t advertiseInterval;
        double activeClientsCheckInterval;
        double asleepClientsCheckInterval;
        double clientsClearInterval;
        double pendingRetainCheckInterval;
        double requestsCheckInterval;
        double registrationsCheckInterval;

        // gateway state management
        inet::ClockEvent* stateChangeEvent = nullptr;
        GatewayState currentState;

        // online gateway state
        inet::ClockEvent* advertiseEvent = nullptr;

        static int gatewayIdCounter;
        uint8_t gatewayId;

        std::map<std::pair<inet::L3Address, int>, ClientInfo> clients;

        inet::ClockEvent* activeClientsCheckEvent = nullptr;
        inet::ClockEvent* asleepClientsCheckEvent = nullptr;
        inet::ClockEvent* clientsClearEvent = nullptr;

        std::map<std::pair<inet::L3Address, int>, PublisherInfo> publishers;

        std::map<std::string, uint16_t> topicsToIds;
        std::map<uint16_t, TopicInfo> idsToTopics;
        std::set<uint16_t> topicIds;
        uint16_t currentTopicId = 0;

        std::map<uint16_t, RetainMessageInfo> retainMessages;
        std::set<uint16_t> retainMessageIds;

        inet::ClockEvent* pendingRetainCheckEvent = nullptr;
        std::map<std::pair<inet::L3Address, int>, MessageInfo> pendingRetainMessages;

        std::map<uint16_t, MessageInfo> messages;
        std::set<uint16_t> messageIds;
        uint16_t currentMessageId = 0;

        inet::ClockEvent* requestsCheckEvent = nullptr;
        std::map<uint16_t, RequestInfo> requests;
        std::set<uint16_t> requestIds;
        uint16_t currentRequestId = 0;

        inet::ClockEvent* registrationsCheckEvent = nullptr;
        std::map<uint16_t, RegisterInfo> registrations;
        std::set<uint16_t> registrationIds;
        uint16_t currentRegistrationId = 0;

        std::map<std::pair<inet::L3Address, int>, SubscriberInfo> subscribers;
        std::map<uint16_t, std::set<QoS>> topicIdToQoS;
        std::map<std::pair<uint16_t, QoS>, std::set<std::pair<inet::L3Address, int>>> subscriptions;

        // statistics
        int numAdvertiseSent = 0;

    protected:
        // initialization
        virtual void levelOneInit() override;

        virtual void finish() override;
        virtual void refreshDisplay() const override;

        // lifecycle
        virtual void handleStartOperation(inet::LifecycleOperation* operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation* operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation* operation) override;

        // message handling
        virtual void handleMessageWhenUp(omnetpp::cMessage* msg) override;

        // gateway state management
        virtual void handleStateChangeEvent();
        virtual void updateCurrentState(GatewayState nextState);
        virtual void scheduleOnlineStateEvents();
        virtual void cancelOnlineStateEvents();
        virtual void cancelOnlineStateClockEvents();

        virtual bool fromOfflineToOnline();
        virtual bool fromOnlineToOffline();

        virtual bool performStateTransition(GatewayState currentState, GatewayState nextState);
        virtual double getStateInterval(GatewayState currentState);
        virtual std::string getGatewayStateAsString();

        // incoming packet handling
        virtual void processPacket(inet::Packet* pk) override;
        virtual void processSearchGw();
        virtual void processConnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillTopic(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, bool isDirectUpdate = false);
        virtual void processWillMsg(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, bool isDirectUpdate = false);
        virtual void processPingReq(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, ClientInfo* clientInfo);
        virtual void processPingResp(const inet::L3Address& srcAddress, const int& srcPort, ClientInfo* clientInfo);
        virtual void processDisconnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, ClientInfo* clientInfo);
        virtual void processRegister(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPublish(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubRel(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processSubscribe(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processUnsubscribe(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubRec(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubComp(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);

        // outgoing packet handling
        virtual void sendAdvertise();

        virtual void sendBaseWithReturnCode(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, ReturnCode returnCode);

        virtual void sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t topicId,
                                              uint16_t msgId, ReturnCode returnCode);

        virtual void sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId);

        virtual void sendSubAck(const inet::L3Address& destAddress, const int& destPort, QoS qosFlag, uint16_t topicId, uint16_t msgId,
                                ReturnCode returnCode);

        virtual void sendRegister(const inet::L3Address& destAddress, const int& destPort, uint16_t topicId, uint16_t msgId,
                                  const std::string& topicName);

        virtual void sendPublish(const inet::L3Address& destAddress, const int& destPort, bool dupFlag, QoS qosFlag, bool retainFlag,
                                 TopicIdType topicIdTypeFlag, uint16_t topicId, uint16_t msgId, const std::string& data);

        // event handlers
        virtual void handleAdvertiseEvent();

        virtual void handleActiveClientsCheckEvent();
        virtual void handleAsleepClientsCheckEvent();
        virtual void handlePendingRetainCheckEvent();
        virtual void handleRequestsCheckEvent();
        virtual void handleRegistrationsCheckEvent();

        virtual void handleClientsClearEvent();

        // client methods
        void cleanClientSession(const inet::L3Address& srcAddress, const int& srcPort, ClientType clientType);
        void updateClientType(ClientInfo* clientInfo, ClientType clientType);
        virtual ClientInfo* addNewClient(const inet::L3Address& srcAddress, const int& srcPort);
        virtual ClientInfo* getClientInfo(const inet::L3Address& srcAddress, const int& srcPort);

        // publisher methods
        virtual PublisherInfo* getPublisherInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);

        // topic methods
        virtual void fillWithPredefinedTopics();
        virtual void addNewTopic(const std::string& topicName, uint16_t topicId, TopicIdType topicIdType);
        virtual void checkTopicsToIds(const std::string& topicName, uint16_t topicId);
        virtual TopicIdType getTopicIdType(uint16_t topicLength);
        virtual TopicInfo getTopicById(uint16_t topicId);
        virtual uint16_t getTopicByName(const std::string& topicName);

        // retain message methods
        virtual void addNewRetainMessage(uint16_t topicId, bool dup, QoS qos, TopicIdType topicIdType, const std::string& data);
        virtual void addNewPendingRetainMessage(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId, QoS qos);

        // request message methods
        virtual void deleteRequestMessageInfo(const RequestInfo& requestInfo, MessageInfo* messageInfo);
        virtual MessageInfo* getRequestMessageInfo(const RequestInfo& requestInfo);

        // request handling methods
        virtual void dispatchPublishToSubscribers(const MessageInfo& messageInfo);

        virtual void addAndSendPublishRequest(const inet::L3Address& subscriberAddress, const int& subscriberPort, const MessageInfo& messageInfo,
                                              QoS requestQoS, uint16_t messagesKey = 0, uint16_t retainMessagesKey = 0);

        virtual void addNewRequest(const inet::L3Address& subscriberAddress, const int& subscriberPort, MsgType messageType,
                                   uint16_t messagesKey = 0, uint16_t retainMessagesKey = 0);

        virtual void deleteRequest(std::map<uint16_t, RequestInfo>::iterator& requestIt, std::set<uint16_t>::iterator& requestIdIt);

        virtual bool isValidRequest(uint16_t requestId, MsgType messageType, std::map<uint16_t, RequestInfo>::iterator& requestIt,
                                    std::set<uint16_t>::iterator& requestIdIt);

        virtual bool processRequestAck(uint16_t requestId, MsgType messageType);

        // registration methods
        virtual void manageRegistration(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId);
        virtual void addNewRegistration(const inet::L3Address& subscriberAddress, const int& subscriberPort);

        virtual void deleteRegistration(std::map<uint16_t, RegisterInfo>::iterator& registrationIt,
                                        std::set<uint16_t>::iterator& registrationIdIt);

        // subscriber methods
        virtual void setAllSubscriberTopics(const inet::L3Address& srcAddress, const int& srcPort, bool isRegistered);
        virtual bool isTopicRegisteredForSubscriber(const inet::L3Address& srcAddress, const int& srcPort, uint16_t topicId);
        virtual SubscriberInfo* getSubscriberInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);

        // subscription methods
        virtual void deleteSubscriptionIfExists(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId);

        virtual bool findSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId,
                                      std::pair<uint16_t, QoS>& subscriptionKey);

        virtual bool insertSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId, QoS qos);

        virtual bool deleteSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                        const std::pair<uint16_t, QoS>& subscriptionKey);

        virtual std::set<std::pair<uint16_t, QoS>> getSubscriptionKeysByTopicId(uint16_t topicId);

        // congestion methods
        virtual bool checkClientsCongestion();
        virtual bool checkIDSpaceCongestion(const std::set<uint16_t>& usedIds, bool allowMaxValue = true);
        virtual bool checkPublishCongestion(QoS qos, bool retain);

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_SERVER_MQTTSNSERVER_H_ */
