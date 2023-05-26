#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        virtual void processPacket(inet::Packet *msg) override;
        virtual void sendPacket();

    public:
        MqttSNClient() {};

        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
