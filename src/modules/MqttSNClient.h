#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"
#include "types/GatewayInfo.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        std::map<uint8_t, GatewayInfo> activeGateways;

    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        virtual void processPacket(inet::Packet *msg) override;
        virtual void processAdvertise(inet::Packet *pk);

    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
