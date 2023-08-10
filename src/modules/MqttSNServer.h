#ifndef MODULES_MQTTSNSERVER_H_
#define MODULES_MQTTSNSERVER_H_

#include "MqttSNApp.h"
#include "types/MsgType.h"
#include "types/ReturnCode.h"
#include "types/ClientInfo.h"

namespace mqttsn {

class MqttSNServer : public MqttSNApp
{
    protected:
        // parameters
        inet::clocktime_t startAdvertise;
        inet::clocktime_t stopAdvertise;
        uint16_t advertiseInterval;

        // state
        inet::ClockEvent *advertiseEvent = nullptr;
        bool lastAdvertise = false;
        bool activeGateway = true;

        static int gatewayIdCounter;
        uint8_t gatewayId;

        std::map<std::pair<inet::L3Address, int>, ClientInfo> connectedClients;

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
        virtual void processSearchGw(inet::Packet *pk);
        virtual void processConnect(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processWillTopic(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processWillMsg(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);

        // send packets
        virtual void sendAdvertise();
        virtual void sendBase(MsgType msgType, inet::L3Address destAddress, int destPort);
        virtual void sendBaseWithReturnCode(MsgType msgType, ReturnCode returnCode, inet::L3Address destAddress, int destPort);

        // event handlers
        virtual void handleAdvertiseEvent();

        // others
        virtual void updateConnectedClients(inet::L3Address srcAddress, int srcPort, std::string clientId);
        virtual bool isClientConnected(inet::L3Address srcAddress, int srcPort);

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNSERVER_H_ */
