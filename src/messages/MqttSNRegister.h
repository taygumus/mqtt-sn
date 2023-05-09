#ifndef MESSAGES_MQTTSNREGISTER_H_
#define MESSAGES_MQTTSNREGISTER_H_

#include "MqttSNMsgIdWithTopicId.h"

namespace mqttsn {

class MqttSNRegister : public MqttSNMsgIdWithTopicId
{
    private:
        std::string topicName;

    public:
        MqttSNRegister() {};

        void setTopicName(std::string name);
        std::string getTopicName() const;

        ~MqttSNRegister() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNREGISTER_H_ */
