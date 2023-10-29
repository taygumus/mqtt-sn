#ifndef HELPERS_PACKETHELPER_H_
#define HELPERS_PACKETHELPER_H_

#include "BaseHelper.h"
#include "inet/common/packet/Packet.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"

namespace mqttsn {

class PacketHelper : public BaseHelper
{
    public:
        static inet::Packet* getRegisterPacket(uint16_t msgId, std::string topicName);
        static inet::Packet* getPublishPacket(bool dupFlag, QoS qosFlag, bool retainFlag, TopicIdType topicIdTypeFlag, uint16_t topicId, uint16_t msgId, std::string data);
};

} /* namespace mqttsn */

#endif /* HELPERS_PACKETHELPER_H_ */
