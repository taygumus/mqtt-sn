#ifndef MESSAGES_MQTTSNCONNACK_H_
#define MESSAGES_MQTTSNCONNACK_H_

#include "MqttSNMessage.h"
#include "types/ReturnCode.h"

namespace mqttsn {

class MqttSNConnAck : public MqttSNMessage
{
    private:
        ReturnCode returnCode;

    public:
        MqttSNConnAck() {};

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode();

        ~MqttSNConnAck() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNCONNACK_H_ */
