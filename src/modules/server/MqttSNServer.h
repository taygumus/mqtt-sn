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
#include "types/server/PublisherInfo.h"
#include "types/server/DataInfo.h"
#include "types/server/RequestInfo.h"

namespace mqttsn {

class MqttSNServer : public MqttSNApp
{
    protected:
        // parameters
        uint16_t advertiseInterval;
        double activeClientsCheckInterval;
        double asleepClientsCheckInterval;
        double clientsClearInterval;

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

        std::map<uint16_t, DataInfo> data;
        std::map<uint16_t, RequestInfo> pendingRequests;

        std::map<uint16_t, std::set<QoS>> topicIdToQos;
        std::map<std::pair<uint16_t, QoS>, std::set<std::pair<inet::L3Address, int>>> subscriptions;

        // statistics
        int numAdvertiseSent = 0;

    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage* msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation* operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation* operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation* operation) override;

        // gateway state management
        virtual void handleStateChangeEvent();
        virtual void scheduleOnlineStateEvents();
        virtual void cancelOnlineStateEvents();
        virtual void updateCurrentState(GatewayState nextState);

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

        // event handlers
        virtual void handleAdvertiseEvent();
        virtual void handleActiveClientsCheckEvent();
        virtual void handleAsleepClientsCheckEvent();
        virtual void handleClientsClearEvent();

        // other methods
        virtual void registerNewTopic(const std::string& topicName);
        virtual bool isGatewayCongested();
        virtual PublisherInfo* getPublisherInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);
        virtual std::set<std::pair<uint16_t, QoS>> getSubscriptionKeysByTopicId(uint16_t topicId);

        // other methods about clients
        virtual void setClientLastMsgTime(const inet::L3Address& srcAddress, const int& srcPort);
        virtual bool isClientInState(const inet::L3Address& srcAddress, const int& srcPort, ClientState clientState);
        virtual ClientInfo* getClientInfo(const inet::L3Address& srcAddress, const int& srcPort, bool insertIfNotFound = false);

        // other methods about subscribers
        virtual void dispatchPublishToSubscribers(uint16_t topicId, QoS qos);

        virtual bool findSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort, uint16_t topicId,
                                      std::pair<uint16_t, QoS>& subscriptionKey);

        virtual bool insertSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                        uint16_t topicId, QoS qos);

        virtual bool deleteSubscription(const inet::L3Address& subscriberAddress, const int& subscriberPort,
                                        const std::pair<uint16_t, QoS>& subscriptionKey);

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_SERVER_MQTTSNSERVER_H_ */
