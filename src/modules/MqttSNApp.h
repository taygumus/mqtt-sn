#ifndef MODULES_MQTTSNAPP_H_
#define MODULES_MQTTSNAPP_H_

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/clock/ClockUserModuleMixin.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

extern template class inet::ClockUserModuleMixin<inet::ApplicationBase>;

namespace mqttsn {

class MqttSNApp : public inet::ClockUserModuleMixin<inet::ApplicationBase>, public inet::UdpSocket::ICallback
{
    protected:
        inet::UdpSocket socket;

    protected:
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
        virtual void initialize(int stage) override = 0;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override = 0;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override = 0;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override = 0;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override = 0;

        virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
        virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
        virtual void socketClosed(inet::UdpSocket *socket) override;

        virtual void processPacket(inet::Packet *msg) = 0;

    public:
        MqttSNApp() {};

        ~MqttSNApp() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
