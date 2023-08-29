#ifndef MODULES_MQTTSNSERVER_H_
#define MODULES_MQTTSNSERVER_H_

#include "MqttSNApp.h"
#include "types/MsgType.h"
#include "types/ReturnCode.h"
#include "types/QoS.h"
#include "types/TopicIdType.h"
#include "types/ClientState.h"
#include "types/ClientInfo.h"
#include "types/ClientInfoUpdates.h"

namespace mqttsn {

class MqttSNServer : public MqttSNApp
{
    protected:
        // parameters
        inet::clocktime_t startAdvertise;
        inet::clocktime_t stopAdvertise;
        uint16_t advertiseInterval;
        double activeClientsCheckInterval;

        // state
        inet::ClockEvent *advertiseEvent = nullptr;
        bool lastAdvertise = false;
        bool activeGateway = true;

        static int gatewayIdCounter;
        uint8_t gatewayId;

        std::map<std::pair<inet::L3Address, int>, ClientInfo> clients;

        inet::ClockEvent *activeClientsCheckEvent = nullptr;

        // statistics
        int numAdvertiseSent = 0;

    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        // process received packets
        virtual void processPacket(inet::Packet *pk) override;
        virtual void processSearchGw();
        virtual void processConnect(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processWillTopic(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processWillMsg(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processPingReq(inet::L3Address srcAddress, int srcPort);
        virtual void processPingResp(inet::L3Address srcAddress, int srcPort);

        // send packets
        virtual void sendAdvertise();
        virtual void sendBaseWithReturnCode(inet::L3Address destAddress, int destPort, MsgType msgType, ReturnCode returnCode);

        // event handlers
        virtual void handleAdvertiseEvent();
        virtual void handleActiveClientsCheckEvent();

        // others
        virtual void updateClientInfo(inet::L3Address srcAddress, int srcPort, ClientInfo& clientInfo, ClientInfoUpdates& updates);
        virtual void applyClientInfoUpdates(ClientInfo& existingClientInfo, ClientInfo& newClientInfo, ClientInfoUpdates& updates);
        virtual bool isGatewayCongested();
        virtual bool isClientExists(inet::L3Address srcAddress, int srcPort, ClientInfo *clientInfo = nullptr);
        virtual bool isClientInState(inet::L3Address srcAddress, int srcPort, ClientState clientState);

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNSERVER_H_ */
