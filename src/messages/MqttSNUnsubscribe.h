#ifndef MESSAGES_MQTTSNUNSUBSCRIBE_H_
#define MESSAGES_MQTTSNUNSUBSCRIBE_H_

#include "MqttSNBaseWithMsgId.h"
#include "types/shared/TopicIdType.h"

namespace mqttsn {

class MqttSNUnsubscribe : public MqttSNBaseWithMsgId
{
    private:
        std::string topicName;
        uint16_t topicId = 0;

    protected:
        uint8_t flags = 0;

    public:
        MqttSNUnsubscribe();

        void setTopicIdTypeFlag(TopicIdType topicIdTypeFlag);
        uint8_t getTopicIdTypeFlag() const;

        void setTopicName(const std::string& name);
        std::string getTopicName() const;

        void setTopicId(uint16_t id);
        uint16_t getTopicId() const;

        ~MqttSNUnsubscribe() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNUNSUBSCRIBE_H_ */
