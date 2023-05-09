#ifndef MESSAGES_MQTTSNUNSUBSCRIBE_H_
#define MESSAGES_MQTTSNUNSUBSCRIBE_H_

#include "MqttSNBaseWithMsgId.h"
#include "types/TopicIdType.h"

namespace mqttsn {

class MqttSNUnsubscribe : public MqttSNBaseWithMsgId
{
    private:
        uint8_t flags = 0;
        std::string topicName;
        uint16_t topicId = 0;

    public:
        MqttSNUnsubscribe();

        void setTopicIdTypeFlag(TopicIdType topicIdTypeFlag);
        uint8_t getTopicIdTypeFlag() const;

        void setTopicName(std::string name);
        std::string getTopicName() const;

        void setTopicId(uint16_t id);
        uint16_t getTopicId() const;

        ~MqttSNUnsubscribe() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNUNSUBSCRIBE_H_ */
