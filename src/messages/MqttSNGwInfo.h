#ifndef MESSAGES_MQTTSNGWINFO_H_
#define MESSAGES_MQTTSNGWINFO_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNGwInfo : public MqttSNBase
{
    private:
        uint8_t gwId = 0;
        uint32_t gwAdd = 0;

    public:
        MqttSNGwInfo();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId() const;

        void setGwAdd(uint32_t gatewayAddress);
        uint32_t getGwAdd() const;

        ~MqttSNGwInfo() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNGWINFO_H_ */
