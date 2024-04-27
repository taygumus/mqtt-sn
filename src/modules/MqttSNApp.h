//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef MODULES_MQTTSNAPP_H_
#define MODULES_MQTTSNAPP_H_

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/clock/ClockUserModuleMixin.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "types/shared/MsgType.h"
#include "types/shared/TopicIdType.h"

extern template class inet::ClockUserModuleMixin<inet::ApplicationBase>;

namespace mqttsn {

class MqttSNApp : public inet::ClockUserModuleMixin<inet::ApplicationBase>, public inet::UdpSocket::ICallback
{
    protected:
        // parameters
        double retransmissionInterval;
        int retransmissionCounter;
        double packetBER;

        // app state
        inet::UdpSocket socket;

        // metrics attributes
        static unsigned serversRetransmissions;

    protected:
        // initialization
        virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
        virtual void initialize(int stage) override;

        // application base
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        // socket handling
        virtual void socketDataArrived(inet::UdpSocket* socket, inet::Packet* packet) override;
        virtual void socketErrorArrived(inet::UdpSocket* socket, inet::Indication* indication) override;
        virtual void socketClosed(inet::UdpSocket* socket) override;
        virtual void socketConfiguration();

        // packet handling
        virtual void checkPacketIntegrity(const inet::B& receivedLength, const inet::B& fieldLength);
        virtual void corruptPacket(inet::Packet* packet, double ber);

        // outgoing packet handling
        virtual void sendGwInfo(uint8_t gatewayId, const std::string& gatewayAddress = "", uint16_t gatewayPort = 0);
        virtual void sendPingReq(const inet::L3Address& destAddress, const int& destPort, const std::string& clientId = "");
        virtual void sendBase(const inet::L3Address& destAddress, const int& destPort, MsgType msgType);
        virtual void sendDisconnect(const inet::L3Address& destAddress, const int& destPort, uint16_t duration = 0);

        // check methods
        virtual bool isSelfBroadcastAddress(const inet::L3Address& address);
        virtual bool hasProbabilisticError(inet::b length, double ber);

        // identifier methods
        virtual bool setNextAvailableId(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue = true);

        virtual uint16_t getNewIdentifier(const std::set<uint16_t>& usedIds, uint16_t& currentId, bool allowMaxValue = true,
                                          const std::string& error = "");

        // topic methods
        virtual void checkTopicLength(uint16_t topicLength, TopicIdType topicIdType);
        virtual bool isMinTopicLength(uint16_t topicLength);
        virtual std::map<std::string, uint16_t> getPredefinedTopics();

        // pure virtual functions
        virtual void levelOneInit() = 0;
        virtual void processPacket(inet::Packet* pk) = 0;

    public:
        MqttSNApp() {};
        ~MqttSNApp() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNAPP_H_ */
