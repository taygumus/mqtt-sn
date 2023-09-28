#ifndef MESSAGES_MQTTSNSUBSCRIBE_H_
#define MESSAGES_MQTTSNSUBSCRIBE_H_

#include "MqttSNUnsubscribe.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class MqttSNSubscribe : public MqttSNUnsubscribe
{
    public:
        MqttSNSubscribe() {};

        void setDupFlag(bool dupFlag);
        bool getDupFlag() const;

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        ~MqttSNSubscribe() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNSUBSCRIBE_H_ */
