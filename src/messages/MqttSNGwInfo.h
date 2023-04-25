#ifndef MESSAGES_MQTTSNGWINFO_H_
#define MESSAGES_MQTTSNGWINFO_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNGwInfo : public MqttSNMessage
{
    private:
        uint8_t gwId;
        uint32_t gwAdd;

    public:
        MqttSNGwInfo() {};

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId();

        void setGwAdd(uint32_t gatewayAddress);
        uint32_t getGwAdd();

        ~MqttSNGwInfo() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNGWINFO_H_ */
