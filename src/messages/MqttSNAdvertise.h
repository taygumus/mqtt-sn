#ifndef MESSAGES_MQTTSNADVERTISE_H_
#define MESSAGES_MQTTSNADVERTISE_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNAdvertise : public MqttSNMessage
{
    private:
        uint8_t gwId = 0;
        uint16_t duration = 0;

    public:
        MqttSNAdvertise();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId();

        void setDuration(uint16_t seconds);
        uint16_t getDuration();

        ~MqttSNAdvertise() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNADVERTISE_H_ */
