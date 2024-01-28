#ifndef MODULES_CLIENT_MQTTSNCLIENT_H_
#define MODULES_CLIENT_MQTTSNCLIENT_H_

#include "../MqttSNApp.h"
#include "types/shared/ClientState.h"
#include "types/shared/MsgType.h"
#include "types/client/GatewayInfo.h"
#include "types/client/RetransmissionInfo.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        // constants
        static constexpr double SEARCH_GATEWAY_MIN_DELAY = 1.1;
        static constexpr double MIN_WAITING_TIME = 0.5;
        static const std::string TOPIC_DELIMITER;

        // parameters
        double checkGatewaysInterval;
        double searchGatewayMaxDelay;
        double searchGatewayInterval;
        uint16_t temporaryDuration;
        double gatewayInfoMaxDelay;
        double gatewayInfoInterval;
        double checkConnectionInterval;
        uint16_t keepAlive;
        double waitingInterval;

        // client state management
        inet::ClockEvent* stateChangeEvent = nullptr;
        ClientState currentState;

        // active client state
        inet::ClockEvent* checkGatewaysEvent = nullptr;
        std::map<uint8_t, GatewayInfo> gateways;

        inet::ClockEvent* searchGatewayEvent = nullptr;
        bool maxIntervalReached = false;
        bool searchGateway = true;

        inet::ClockEvent* gatewayInfoEvent = nullptr;

        inet::ClockEvent* checkConnectionEvent = nullptr;
        std::string clientId;
        bool isConnected = false;
        GatewayInfo selectedGateway;

        inet::ClockEvent* pingEvent = nullptr;

        uint16_t currentMsgId = 0;

        std::map<std::string, uint16_t> predefinedTopics;

        // retransmission management
        std::map<MsgType, RetransmissionInfo> retransmissions;

        // metrics attributes
        static double sumReceivedPublishMsgTimestamps;
        static unsigned receivedTotalPublishMsgs;

        static unsigned sentUniquePublishMsgs;
        static unsigned receivedUniquePublishMsgs;

    protected:
        // initialization
        virtual void levelOneInit() override;

        // application base
        virtual void finish() override;

        // lifecycle
        virtual void handleStartOperation(inet::LifecycleOperation* operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation* operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation* operation) override;

        // message handling
        virtual void handleMessageWhenUp(omnetpp::cMessage* msg) override;

        // client state management
        virtual void handleStateChangeEvent();
        virtual void updateCurrentState(ClientState nextState);
        virtual void returnToSleep();
        virtual void scheduleActiveStateEvents();
        virtual void cancelActiveStateEvents();
        virtual void cancelActiveStateClockEvents();

        virtual bool fromDisconnectedToActive();
        virtual bool fromLostToActive();
        virtual bool fromActiveToDisconnected();
        virtual bool fromActiveToLost();
        virtual bool fromActiveToAsleep();
        virtual bool fromAsleepToLost();
        virtual bool fromAsleepToActive();
        virtual bool fromAsleepToAwake();
        virtual bool fromAsleepToDisconnected();

        virtual bool performStateTransition(ClientState currentState, ClientState nextState);
        virtual double getStateInterval(ClientState currentState);
        virtual std::string getClientStateAsString();
        virtual std::vector<ClientState> getNextPossibleStates(ClientState currentState);

        // incoming packet handling
        virtual void processPacket(inet::Packet* pk) override;
        virtual void processPacketByMessageType(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType);
        virtual bool isValidPacket(const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType);

        // incoming packet type methods
        virtual void processAdvertise(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processSearchGw();
        virtual void processGwInfo(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processConnAck(inet::Packet* pk);
        virtual void processPingReq(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPingResp(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processDisconnect(inet::Packet* pk);

        // outgoing packet handling
        virtual void sendSearchGw();
        virtual void sendConnect(const inet::L3Address& destAddress, const int& destPort, bool willFlag, bool cleanSessionFlag, uint16_t duration);

        // event handlers
        virtual void handleCheckGatewaysEvent();
        virtual void handleSearchGatewayEvent();
        virtual void handleGatewayInfoEvent();
        virtual void handleCheckConnectionEvent();
        virtual void handlePingEvent();

        // gateway methods
        virtual void updateActiveGateways(const inet::L3Address& srcAddress, const int& srcPort, uint8_t gatewayId, uint16_t duration);
        virtual bool isSelectedGateway(const inet::L3Address& srcAddress, const int& srcPort);
        virtual bool isConnectedGateway(const inet::L3Address& srcAddress, const int& srcPort);
        virtual std::pair<uint8_t, GatewayInfo> selectGateway();

        // client identifier methods
        virtual std::string generateClientId();

        // message identifier methods
        virtual void scheduleRetransmissionWithMsgId(MsgType msgType, uint16_t msgId);
        virtual bool checkMsgIdForType(MsgType msgType, uint16_t msgId);
        virtual bool processAckForMsgType(MsgType msgType, uint16_t msgId);
        virtual uint16_t getNewMsgId();
        virtual std::set<uint16_t> getUsedMsgIds();

        // topic methods
        virtual void checkTopicConsistency(const std::string& topicName, TopicIdType topicIdType, bool isFound);
        virtual uint16_t getPredefinedTopicId(const std::string& topicName);

        // result handling
        virtual void handleFinalSimulationResults();
        virtual void printStatistics();
        virtual void computePublishEndToEndDelay();
        virtual void computePublishHitRate();

        // retransmission management
        virtual void scheduleMsgRetransmission(const inet::L3Address& destAddress, const int& destPort, MsgType msgType,
                                               std::map<std::string, std::string>* parameters = nullptr);

        virtual void unscheduleMsgRetransmission(MsgType msgType);
        virtual void clearRetransmissions();
        virtual void handleRetransmissionEvent(omnetpp::cMessage* msg);

        virtual void retransmitDisconnect(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitPingReq(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);

        // pure virtual functions
        virtual void levelTwoInit() = 0;
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage* msg) = 0;

        virtual void scheduleActiveStateEventsCustom() = 0;
        virtual void cancelActiveStateEventsCustom() = 0;
        virtual void cancelActiveStateClockEventsCustom() = 0;

        virtual void adjustAllowedPacketTypes(std::vector<MsgType>& msgTypes) = 0;
        virtual void processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType) = 0;
        virtual void processConnAckCustom() = 0;
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) = 0;
        virtual void populateItems() = 0;

        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg,
                                                     MsgType msgType) = 0;
    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNCLIENT_H_ */
