#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"
#include "types/ClientState.h"
#include "types/MsgType.h"
#include "types/GatewayInfo.h"
#include "types/QoS.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        // parameters
        double checkGatewaysInterval;
        double searchGatewayInterval;
        uint16_t temporaryDuration;
        double gatewayInfoInterval;
        double checkConnectionInterval;

        // client state management
        inet::ClockEvent *stateChangeEvent = nullptr;
        ClientState currentState;

        // active state
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
        virtual double getStateInterval(ClientState currentState);
        virtual std::string getClientState();
        virtual std::vector<ClientState> getNextPossibleStates(ClientState currentState);

        // process received packets
        virtual void processPacket(inet::Packet *pk) override;
        virtual void processAdvertise(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processSearchGw(inet::Packet *pk);
        virtual void processGwInfo(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processConnAck(inet::Packet *pk);
        virtual void processWillTopicReq(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processWillMsgReq(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);

        // send packets
        virtual void sendSearchGw();
        virtual void sendConnect(bool willFlag, bool cleanSessionFlag, uint16_t duration, inet::L3Address destAddress, int destPort);
        virtual void sendBaseWithWillTopic(MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic, inet::L3Address destAddress, int destPort);
        virtual void sendBaseWithWillMsg(MsgType msgType, std::string willMsg, inet::L3Address destAddress, int destPort);

        // event handlers
        virtual void handleCheckGatewaysEvent();
        virtual void handleSearchGatewayEvent();
        virtual void handleGatewayInfoEvent();
        virtual void handleCheckConnectionEvent();

        // others
        virtual void checkGatewaysAvailability();
        virtual void updateActiveGateways(uint8_t gatewayId, uint16_t duration, inet::L3Address srcAddress, int srcPort);
        virtual bool isSelectedGateway(inet::L3Address srcAddress, int srcPort);
        virtual bool isConnectedGateway(inet::L3Address srcAddress, int srcPort);
        virtual std::string generateClientId();
        virtual std::pair<uint8_t, GatewayInfo> selectGateway();
        virtual QoS intToQoS(int value);

    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
