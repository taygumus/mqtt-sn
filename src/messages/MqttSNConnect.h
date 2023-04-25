#ifndef MESSAGES_MQTTSNCONNECT_H_
#define MESSAGES_MQTTSNCONNECT_H_

#include "MqttSNMessage.h"
#include "types/QoS.h"
#include "types/TopicIdType.h"

namespace mqttsn {

class MqttSNConnect : public MqttSNMessage
{
    private:
        uint8_t flags;

    public:
        MqttSNConnect() {};

        void setDupFlag(bool dupFlag);
        bool getDupFlag();

        void setQoSFlag(QoS level);
        QoS getQoSFlag();

        void setRetainFlag(bool retainFlag);
        bool getRetainFlag();

        void setWillFlag(bool willFlag);
        bool getWillFlag();

        void setCleanSessionFlag(bool cleanSessionFlag);
        bool getCleanSessionFlag();

        void setTopicIdTypeFlag(TopicIdType topicIdType);
        TopicIdType getTopicIdTypeFlag();

        ~MqttSNConnect() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNCONNECT_H_ */
