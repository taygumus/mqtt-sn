#ifndef MODULES_SERVER_MQTTSNSERVER_H_
#define MODULES_SERVER_MQTTSNSERVER_H_

#include "../MqttSNApp.h"
#include "types/shared/MsgType.h"
#include "types/shared/ReturnCode.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/shared/ClientState.h"
#include "types/server/GatewayState.h"
#include "types/server/ClientInfo.h"
#include "types/server/DataInfo.h"
#include "types/server/PublisherInfo.h"
#include "types/server/MessageInfo.h"
#include "types/server/RequestInfo.h"
#include "types/server/RetainMessageInfo.h"

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
        std::set<uint16_t> topicIds;
        uint16_t currentTopicId = 0;

        inet::ClockEvent* pendingRetainCheckEvent = nullptr;
        std::map<std::pair<inet::L3Address, int>, MessageInfo> pendingRetainMessages;

        std::map<uint16_t, RetainMessageInfo> retainMessages;
        std::set<uint16_t> retainMessageIds;

        std::map<uint16_t, MessageInfo> messages;
        std::set<uint16_t> messageIds;
        uint16_t currentMessageId = 0;

        inet::ClockEvent* requestsCheckEvent = nullptr;
        std::map<uint16_t, RequestInfo> requests;
        std::set<uint16_t> requestIds;
        uint16_t currentRequestId = 0;

        std::map<uint16_t, std::set<QoS>> topicIdToQoS;
        std::map<std::pair<uint16_t, QoS>, std::set<std::pair<inet::L3Address, int>>> subscriptions;

        // statistics
        int numAdvertiseSent = 0;

    protected:
        virtual void levelOneInit() override;
        virtual void handleMessageWhenUp(omnetpp::cMessage* msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation* operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation* operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation* operation) override;

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

        // process received packets
        virtual void processPacket(inet::Packet* pk) override;
        virtual void processSearchGw();
        virtual void processConnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillTopic(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, bool isDirectUpdate = false);
        virtual void processWillMsg(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, bool isDirectUpdate = false);
        virtual void processPingReq(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPingResp(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processDisconnect(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processRegister(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPublish(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubRel(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processSubscribe(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processUnsubscribe(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubAck(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubRec(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubComp(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);

        // send packets
        virtual void sendAdvertise();

        virtual void sendBaseWithReturnCode(const inet::L3Address& destAddress, const int& destPort,
                                            MsgType msgType, ReturnCode returnCode);

        virtual void sendMsgIdWithTopicIdPlus(const inet::L3Address& destAddress, const int& destPort,
                                              MsgType msgType, ReturnCode returnCode,
                                              uint16_t topicId, uint16_t msgId);

        virtual void sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId);

        virtual void sendSubAck(const inet::L3Address& destAddress, const int& destPort,
                                QoS qosFlag, ReturnCode returnCode,
                                uint16_t topicId, uint16_t msgId);

        virtual void sendPublish(const inet::L3Address& destAddress, const int& destPort,
                                 bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag,
                                 uint16_t topicId, uint16_t msgId,
                                 const std::string& data);

        // event handlers
        virtual void handleAdvertiseEvent();

        virtual void handleActiveClientsCheckEvent();
        virtual void handleAsleepClientsCheckEvent();
        virtual void handlePendingRetainCheckEvent();
        virtual void handleRequestsCheckEvent();

        virtual void handleClientsClearEvent();

        // other methods
        virtual void fillWithPredefinedTopics();
        virtual void registerNewTopic(const std::string& topicName);
        virtual void addNewRetainMessage(uint16_t topicId, bool dup, QoS qos, TopicIdType topicIdType, const std::string& data);

        // other methods about congestions
        virtual bool checkClientsCongestion();
        virtual bool checkIDSpaceCongestion(const std::set<uint16_t>& usedIds, bool allowMaxValue = true);
        virtual bool checkPublishCongestion(QoS qosFlag, bool retainFlag);

        // other methods about clients
        virtual void setClientLastMsgTime(const inet::L3Address& srcAddress, const int& srcPort);
        virtual bool isClientInState(const inet::L3Address& srcAddress, const int& srcPort, ClientState clientState);
        virtual ClientInfo* getClientInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);

        // other methods about publishers
        virtual PublisherInfo* getPublisherInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);

        // other methods about subscribers
        virtual void addNewPendingRetainMessage(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                                uint16_t topicId, QoS qos);

        // other methods about subscribers; handling requests
        virtual void dispatchPublishToSubscribers(const MessageInfo& messageInfo);

        virtual void saveAndSendPublishRequest(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                               const MessageInfo& messageInfo, QoS requestQoS,
                                               uint16_t messagesKey = 0, uint16_t retainMessagesKey = 0);

        virtual void addNewRequest(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                   MsgType messageType, uint16_t messagesKey = 0, uint16_t retainMessagesKey = 0);

        virtual void deleteRequest(std::map<uint16_t, RequestInfo>::iterator& requestIt, std::set<uint16_t>::iterator& requestIdIt);

        virtual bool isValidRequest(uint16_t requestId, MsgType messageType,
                                    std::map<uint16_t, RequestInfo>::iterator& requestIt, std::set<uint16_t>::iterator& requestIdIt);

        virtual bool processRequestAck(uint16_t requestId, MsgType messageType);

        // other methods about subscribers; handling request messages
        virtual void deleteRequestMessageInfo(const RequestInfo& requestInfo, MessageInfo* messageInfo);
        virtual MessageInfo* getRequestMessageInfo(const RequestInfo& requestInfo);

        // other methods about subscribers subscriptions
        virtual bool findSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId,
                                      std::pair<uint16_t, QoS>& subscriptionKey);

        virtual bool insertSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                        uint16_t topicId, QoS qos);

        virtual bool deleteSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                        const std::pair<uint16_t, QoS>& subscriptionKey);

        virtual std::set<std::pair<uint16_t, QoS>> getSubscriptionKeysByTopicId(uint16_t topicId);

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_SERVER_MQTTSNSERVER_H_ */
