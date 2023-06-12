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

        virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
        virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
        virtual void socketClosed(inet::UdpSocket *socket) override;

        // process received packet
        virtual void processPacket(inet::Packet *pk) = 0;

        // send packet
        virtual void sendGwInfo(uint8_t gatewayId, uint32_t gatewayAddress = 0);

        // others
        virtual void checkPacketIntegrity(inet::B receivedLength, inet::B fieldLength);
        bool isSelfBroadcastAddress(inet::L3Address address);

    public:
        MqttSNApp() {};
        ~MqttSNApp() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
