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

#include "PacketHelper.h"
#include "inet/common/TimeTag_m.h"
#include "tags/IdentifierTag.h"
#include "messages/MqttSNRegister.h"
#include "messages/MqttSNPublish.h"
#include "messages/MqttSNBaseWithMsgId.h"
#include "messages/MqttSNMsgIdWithTopicIdPlus.h"

namespace mqttsn {

inet::Packet* PacketHelper::getRegisterPacket(uint16_t topicId, uint16_t msgId, const std::string& topicName)
{
    const auto& payload = inet::makeShared<MqttSNRegister>();
    payload->setMsgType(MsgType::REGISTER);
    payload->setTopicId(topicId);
    payload->setMsgId(msgId);
    payload->setTopicName(topicName);
    payload->setChunkLength(inet::B(payload->getLength()));

    inet::Packet* packet = new inet::Packet("RegisterPacket");
    packet->insertAtBack(payload);

    return packet;
}

inet::Packet* PacketHelper::getPublishPacket(bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag, uint16_t topicId,
                                             uint16_t msgId, const std::string& data, const TagInfo& tagInfo)
{
    const auto& payload = inet::makeShared<MqttSNPublish>();
    payload->setMsgType(MsgType::PUBLISH);
    payload->setDupFlag(dupFlag);
    payload->setQoSFlag(qosFlag);
    payload->setRetainFlag(retainFlag);
    payload->setTopicIdTypeFlag(topicIdTypeFlag);
    payload->setTopicId(topicId);
    payload->setMsgId(msgId);
    payload->setData(data);
    payload->setChunkLength(inet::B(payload->getLength()));

    if (tagInfo.timestamp > 0) {
        payload->addTag<inet::CreationTimeTag>()->setCreationTime(CLOCKTIME_AS_SIMTIME(tagInfo.timestamp));
    }

    if (tagInfo.identifier > 0) {
        payload->addTag<IdentifierTag>()->setIdentifier(tagInfo.identifier);
    }

    inet::Packet* packet = new inet::Packet("PublishPacket");
    packet->insertAtBack(payload);

    return packet;
}

inet::Packet* PacketHelper::getBaseWithMsgIdPacket(MsgType msgType, uint16_t msgId)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithMsgId>();
    payload->setMsgType(msgType);
    payload->setMsgId(msgId);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::PUBREC:
            packetName = "PubRecPacket";
            break;

        case MsgType::PUBREL:
            packetName = "PubRelPacket";
            break;

        case MsgType::PUBCOMP:
            packetName = "PubCompPacket";
            break;

        case MsgType::UNSUBACK:
            packetName = "UnsubAckPacket";
            break;

        default:
            packetName = "BaseWithMsgId";
    }

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    return packet;
}

inet::Packet* PacketHelper::getMsgIdWithTopicIdPlusPacket(MsgType msgType, uint16_t topicId, uint16_t msgId, ReturnCode returnCode)
{
    const auto& payload = inet::makeShared<MqttSNMsgIdWithTopicIdPlus>();
    payload->setMsgType(msgType);
    payload->setTopicId(topicId);
    payload->setMsgId(msgId);
    payload->setReturnCode(returnCode);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::REGACK:
            packetName = "RegAckPacket";
            break;

        case MsgType::PUBACK:
            packetName = "PubAckPacket";
            break;

        default:
            packetName = "MsgIdWithTopicIdPlus";
    }

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    return packet;
}

} /* namespace mqttsn */
