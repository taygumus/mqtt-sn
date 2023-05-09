#ifndef MESSAGES_MQTTSNMSGIDWITHTOPICID_H_
#define MESSAGES_MQTTSNMSGIDWITHTOPICID_H_

#include "MqttSNBaseWithMsgId.h"

namespace mqttsn {

class MqttSNMsgIdWithTopicId : public MqttSNBaseWithMsgId
{
    private:
        uint16_t topicId = 0;

    public:
        MqttSNMsgIdWithTopicId();

        void setTopicId(uint16_t id);
        uint16_t getTopicId() const;

        ~MqttSNMsgIdWithTopicId() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMSGIDWITHTOPICID_H_ */
