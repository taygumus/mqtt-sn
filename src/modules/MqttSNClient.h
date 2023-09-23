#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"
#include "types/ClientState.h"
#include "types/MsgType.h"
#include "types/QoS.h"
#include "types/client/GatewayInfo.h"
#include "types/client/UnicastMessageInfo.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        // constants
        static constexpr double SEARCH_GATEWAY_MIN_DELAY = 1.1;

        // parameters
        double checkGatewaysInterval;
        double searchGatewayMaxDelay;
        double searchGatewayInterval;
        uint16_t temporaryDuration;
        double gatewayInfoMaxDelay;
        double gatewayInfoInterval;
        double checkConnectionInterval;
        uint16_t keepAlive;
        double retransmissionInterval;

        // client state management
        inet::ClockEvent *stateChangeEvent = nullptr;
        ClientState currentState;

        // active client state
        inet::ClockEvent *checkGatewaysEvent = nullptr;
        std::map<uint8_t, GatewayInfo> activeGateways;

        inet::ClockEvent *searchGatewayEvent = nullptr;
        bool maxIntervalReached = false;
        bool searchGateway = true;

        inet::ClockEvent *gatewayInfoEvent = nullptr;

        inet::ClockEvent *checkConnectionEvent = nullptr;
        std::string clientId;
        bool isConnected = false;
        GatewayInfo selectedGateway;

        inet::ClockEvent *pingEvent = nullptr;

        // retransmissions management
        std::map<MsgType, UnicastMessageInfo> retransmissions;

    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        // client state management
        virtual void handleStateChangeEvent();
        virtual void scheduleActiveStateEvents();
        virtual void cancelActiveStateEvents();
        virtual void updateCurrentState(ClientState nextState);
        virtual void returnToSleep();

        virtual bool fromDisconnectedToActive();
        virtual bool fromActiveToDisconnected();
        virtual bool fromActiveToLost();
        virtual bool fromLostToActive();
        virtual bool fromActiveToAsleep();
        virtual bool fromAsleepToLost();
        virtual bool fromAsleepToActive();
        virtual bool fromAsleepToAwake();
        virtual bool fromAsleepToDisconnected();

        virtual bool performStateTransition(ClientState currentState, ClientState nextState);
        virtual double getStateInterval(ClientState currentState);
        virtual std::string getClientStateAsString();
        virtual std::vector<ClientState> getNextPossibleStates(ClientState currentState);

        // process received packets
        virtual void processPacket(inet::Packet *pk) override;
        virtual void processAdvertise(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processSearchGw();
        virtual void processGwInfo(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processConnAck(inet::Packet *pk);
        virtual void processWillTopicReq(inet::L3Address srcAddress, int srcPort);
        virtual void processWillMsgReq(inet::L3Address srcAddress, int srcPort);
        virtual void processPingReq(inet::L3Address srcAddress, int srcPort);
        virtual void processPingResp(inet::L3Address srcAddress, int srcPort);
        virtual void processDisconnect(inet::Packet *pk);

        // send packets
        virtual void sendSearchGw();
        virtual void sendConnect(inet::L3Address destAddress, int destPort, bool willFlag, bool cleanSessionFlag, uint16_t duration);
        virtual void sendBaseWithWillTopic(inet::L3Address destAddress, int destPort, MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic);
        virtual void sendBaseWithWillMsg(inet::L3Address destAddress, int destPort, MsgType msgType, std::string willMsg);

        // event handlers
        virtual void handleCheckGatewaysEvent();
        virtual void handleSearchGatewayEvent();
        virtual void handleGatewayInfoEvent();
        virtual void handleCheckConnectionEvent();
        virtual void handlePingEvent();

        // others
        virtual void updateActiveGateways(inet::L3Address srcAddress, int srcPort, uint8_t gatewayId, uint16_t duration);
        virtual bool isSelectedGateway(inet::L3Address srcAddress, int srcPort);
        virtual bool isConnectedGateway(inet::L3Address srcAddress, int srcPort);
        virtual std::string generateClientId();
        virtual std::pair<uint8_t, GatewayInfo> selectGateway();
        virtual QoS intToQoS(int value);

        // retransmissions management
        virtual void scheduleMsgRetransmission(MsgType msgType, inet::L3Address destAddress, int destPort, std::map<std::string, std::string>* parameters = nullptr);
        virtual void unscheduleMsgRetransmission(MsgType msgType);
        virtual void clearRetransmissions();
        virtual void handleRetransmissionEvent(omnetpp::cMessage *msg);

        virtual void retransmitDisconnect(inet::L3Address destAddress, int destPort, omnetpp::cMessage *msg, bool retransmission = true);
        virtual void retransmitPingReq(inet::L3Address destAddress, int destPort, omnetpp::cMessage *msg, bool retransmission = true);

    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
