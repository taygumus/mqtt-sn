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
        // application state
        inet::UdpSocket socket;
        std::map<std::string, uint16_t> predefinedTopics;

    protected:
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }

        virtual void initialize(int stage) override;
        virtual void socketDataArrived(inet::UdpSocket* socket, inet::Packet* packet) override;
        virtual void socketErrorArrived(inet::UdpSocket* socket, inet::Indication* indication) override;
        virtual void socketClosed(inet::UdpSocket* socket) override;

        // send packets
        virtual void sendGwInfo(uint8_t gatewayId, const std::string& gatewayAddress = "", uint16_t gatewayPort = 0);
        virtual void sendPingReq(const inet::L3Address& destAddress, const int& destPort, const std::string& clientId = "");
        virtual void sendBase(const inet::L3Address& destAddress, const int& destPort, MsgType msgType);
        virtual void sendDisconnect(const inet::L3Address& destAddress, const int& destPort, uint16_t duration = 0);

        // other methods
        virtual void checkPacketIntegrity(const inet::B& receivedLength, const inet::B& fieldLength);
        virtual bool isSelfBroadcastAddress(const inet::L3Address& address);

        // other methods about identifiers
        virtual bool setNextAvailableId(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue = true);

        virtual uint16_t getNewIdentifier(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue = true,
                                          const std::string& error = "");

        // other methods about predefined topics
        virtual void fillPredefinedTopics();
        virtual uint16_t getPredefinedTopicId(const std::string& topicName);

        // pure virtual functions
        virtual void levelOneInit() = 0;
        virtual void processPacket(inet::Packet* pk) = 0;

    public:
        MqttSNApp() {};
        ~MqttSNApp() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
