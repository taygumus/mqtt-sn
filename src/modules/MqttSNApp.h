#ifndef MODULES_MQTTSNAPP_H_
#define MODULES_MQTTSNAPP_H_

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/clock/ClockUserModuleMixin.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "types/shared/MsgType.h"

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

        // process received packets
        virtual void processPacket(inet::Packet *pk) = 0;

        // send packets
        virtual void sendGwInfo(uint8_t gatewayId, std::string gatewayAddress = "", uint16_t gatewayPort = 0);
        virtual void sendPingReq(inet::L3Address destAddress, int destPort, std::string clientId = "");
        virtual void sendBase(inet::L3Address destAddress, int destPort, MsgType msgType);
        virtual void sendDisconnect(inet::L3Address destAddress, int destPort, uint16_t duration = 0);

        // other functions
        virtual void checkPacketIntegrity(inet::B receivedLength, inet::B fieldLength);
        virtual bool isSelfBroadcastAddress(inet::L3Address address);

    public:
        MqttSNApp() {};
        ~MqttSNApp() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
