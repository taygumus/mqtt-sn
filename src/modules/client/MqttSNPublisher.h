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

#ifndef MODULES_CLIENT_MQTTSNPUBLISHER_H_
#define MODULES_CLIENT_MQTTSNPUBLISHER_H_

#include "MqttSNClient.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/shared/TagInfo.h"
#include "types/client/publisher/DataInfo.h"
#include "types/client/publisher/ItemInfo.h"
#include "types/client/publisher/TopicInfo.h"
#include "types/client/publisher/LastRegisterInfo.h"
#include "types/client/publisher/LastPublishInfo.h"

namespace mqttsn {

class MqttSNPublisher : public MqttSNClient
{
    protected:
        // parameters
        int willQoS;
        bool willRetain;
        std::string willTopic;
        std::string willMsg;
        double registrationInterval;
        double publishInterval;
        double publishMinusOneInterval;
        inet::L3Address publishMinusOneDestAddress;
        int publishMinusOneDestPort;

        // active publisher state
        std::map<int, ItemInfo> items;

        inet::ClockEvent* registrationEvent = nullptr;
        LastRegisterInfo lastRegistration;
        int registrationCounter = 0;

        std::map<uint16_t, TopicInfo> topics;

        inet::ClockEvent* publishEvent = nullptr;
        LastPublishInfo lastPublish;
        int publishCounter = 0;

        inet::ClockEvent* publishMinusOneEvent = nullptr;
        LastPublishInfo lastPublishMinusOne;
        int publishMinusOneCounter = 0;

        // metrics attributes
        static unsigned publishMsgIdentifier;

    protected:
        // initialization
        virtual void levelTwoInit() override;

        // message handling
        virtual bool handleMessageWhenUpCustom(omnetpp::cMessage* msg) override;

        // active state management
        virtual void scheduleActiveStateEventsCustom() override;
        virtual void cancelActiveStateEventsCustom() override;
        virtual void cancelActiveStateClockEventsCustom() override;

        // incoming packet handling
        virtual void adjustAllowedPacketTypes(std::vector<MsgType>& msgTypes) override {};
        virtual void processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType) override;
        virtual void processConnAckCustom() override;

        // incoming packet type methods
        virtual void processWillTopicReq(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillMsgReq(const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processWillResp(inet::Packet* pk, bool willTopic);
        virtual void processRegAck(inet::Packet* pk);
        virtual void processPubAck(inet::Packet* pk);
        virtual void processPubRec(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort);
        virtual void processPubComp(inet::Packet* pk);

        // outgoing packet handling
        virtual void sendBaseWithWillTopic(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, QoS qosFlag,
                                           bool retainFlag, const std::string& willTopic);

        virtual void sendBaseWithWillMsg(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, const std::string& willMsg);
        virtual void sendRegister(const inet::L3Address& destAddress, const int& destPort, uint16_t msgId, const std::string& topicName);

        virtual void sendPublish(const inet::L3Address& destAddress, const int& destPort, bool dupFlag, QoS qosFlag, bool retainFlag,
                                 TopicIdType topicIdTypeFlag, uint16_t topicId, uint16_t msgId, const std::string& data, const TagInfo& tagInfo);

        virtual void sendBaseWithMsgId(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, uint16_t msgId);

        // event handlers
        virtual void handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort) override;
        virtual void handleRegistrationEvent();
        virtual void handlePublishEvent();
        virtual void handlePublishMinusOneEvent();

        // gateway methods
        virtual void validatePublishMinusOneGateway();

        // item methods
        virtual void populateItems() override;

        // topic methods
        virtual void resetAndPopulateTopics();
        virtual bool proceedWithRegistration();

        // publication methods
        virtual void printPublishMessage(const LastPublishInfo& lastPublishInfo);
        virtual void retryLastPublish();
        virtual bool proceedWithPublish();
        virtual bool proceedWithPublishMinusOne();

        // retransmission management
        virtual void handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg,
                                                     MsgType msgType) override;

        virtual void updateRetransmissionsCounter() override;

        virtual void retransmitWillTopicUpd(const inet::L3Address& destAddress, const int& destPort);
        virtual void retransmitWillMsgUpd(const inet::L3Address& destAddress, const int& destPort);
        virtual void retransmitRegister(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitPublish(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);
        virtual void retransmitPubRel(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg);

    public:
        MqttSNPublisher() {};
        ~MqttSNPublisher();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNPUBLISHER_H_ */
