#ifndef MESSAGES_MQTTSNADVERTISE_H_
#define MESSAGES_MQTTSNADVERTISE_H_

#include "MqttSNBaseWithDuration.h"

namespace mqttsn {

class MqttSNAdvertise : public MqttSNBaseWithDuration
{
    private:
        uint8_t gwId = 0;

    public:
        MqttSNAdvertise();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId() const;

        ~MqttSNAdvertise() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNADVERTISE_H_ */
