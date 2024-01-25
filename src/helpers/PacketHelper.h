#ifndef HELPERS_PACKETHELPER_H_
#define HELPERS_PACKETHELPER_H_

#include "BaseHelper.h"
#include "inet/common/clock/ClockUserModuleMixin.h"
#include "inet/common/packet/Packet.h"
#include "types/shared/MsgType.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"
#include "types/shared/ReturnCode.h"
#include "types/shared/TagInfo.h"

namespace mqttsn {

class PacketHelper : public BaseHelper
{
    public:
        static inet::Packet* getRegisterPacket(uint16_t topicId, uint16_t msgId, const std::string& topicName);

        static inet::Packet* getPublishPacket(bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag, uint16_t topicId,
                                              uint16_t msgId, const std::string& data, const TagInfo& tagInfo);

        static inet::Packet* getBaseWithMsgIdPacket(MsgType msgType, uint16_t msgId);
        static inet::Packet* getMsgIdWithTopicIdPlusPacket(MsgType msgType, uint16_t topicId, uint16_t msgId, ReturnCode returnCode);
};

} /* namespace mqttsn */

#endif /* HELPERS_PACKETHELPER_H_ */
