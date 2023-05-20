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
        enum SelfMsgKinds { START = 1, SEND, STOP };

        inet::UdpSocket socket;
        inet::ClockEvent *selfMsg = nullptr;

    protected:
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }

        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
        virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
        virtual void socketClosed(inet::UdpSocket *socket) override;

        virtual void processStart() = 0;
        virtual void processSend() = 0;
        virtual void processStop() = 0;
        virtual void processPacket(inet::Packet *msg) = 0;

    public:
        MqttSNApp() {};
        ~MqttSNApp();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
