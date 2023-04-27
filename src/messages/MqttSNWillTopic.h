#ifndef MESSAGES_MQTTSNWILLTOPIC_H_
#define MESSAGES_MQTTSNWILLTOPIC_H_

#include "MqttSNMessage.h"
#include "types/QoS.h"

namespace mqttsn {

class MqttSNWillTopic : public MqttSNMessage
{
    private:
        uint8_t flags = 0;
        std::string willTopic;

    public:
        MqttSNWillTopic() {};

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag();

        void setRetainFlag(bool retainFlag);
        bool getRetainFlag();

        void setWillTopic(std::string topicName);
        std::string getWillTopic();

        ~MqttSNWillTopic() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNWILLTOPIC_H_ */
