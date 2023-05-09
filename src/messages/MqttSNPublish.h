#ifndef MESSAGES_MQTTSNPUBLISH_H_
#define MESSAGES_MQTTSNPUBLISH_H_

#include "MqttSNMsgIdWithTopicId.h"
#include "types/QoS.h"
#include "types/TopicIdType.h"

namespace mqttsn {

class MqttSNPublish : public MqttSNMsgIdWithTopicId
{
    private:
        uint8_t flags = 0;
        std::string data;

    public:
        MqttSNPublish();

        void setDupFlag(bool dupFlag);
        bool getDupFlag() const;

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        void setRetainFlag(bool retainFlag);
        bool getRetainFlag() const;

        void setTopicIdTypeFlag(TopicIdType topicIdTypeFlag);
        uint8_t getTopicIdTypeFlag() const;

        void setData(std::string stringData);
        std::string getData() const;

        ~MqttSNPublish() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNPUBLISH_H_ */
