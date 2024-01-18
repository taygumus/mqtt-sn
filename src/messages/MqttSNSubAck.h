#ifndef MESSAGES_MQTTSNSUBACK_H_
#define MESSAGES_MQTTSNSUBACK_H_

#include "MqttSNMsgIdWithTopicIdPlus.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class MqttSNSubAck : public MqttSNMsgIdWithTopicIdPlus
{
    private:
        uint8_t flags = 0;

    public:
        MqttSNSubAck();

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        ~MqttSNSubAck() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNSUBACK_H_ */
