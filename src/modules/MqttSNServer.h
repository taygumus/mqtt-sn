#ifndef MODULES_MQTTSNSERVER_H_
#define MODULES_MQTTSNSERVER_H_

#include "MqttSNApp.h"
#include "types/MsgType.h"

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

        // process received packet
        virtual void processPacket(inet::Packet *pk) override;
        virtual void processSearchGw(inet::Packet *pk);

        // send packet
        virtual void sendAdvertise();

        // event handler
        virtual void handleAdvertiseEvent();

    public:
        MqttSNServer() {};
        ~MqttSNServer();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNSERVER_H_ */
