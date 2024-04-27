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
