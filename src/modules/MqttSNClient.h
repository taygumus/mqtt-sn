#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"
#include "types/GatewayInfo.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        // parameters
        volatile double checkGatewaysInterval;

        // state
        inet::ClockEvent *checkGatewaysEvent = nullptr;
        std::map<uint8_t, GatewayInfo> activeGateways;

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
        virtual void processAdvertise(inet::Packet *pk);

        // send packet
            //
        virtual void checkGatewaysAvailability();

    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
