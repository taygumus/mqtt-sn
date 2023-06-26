#ifndef MESSAGES_MQTTSNGWINFO_H_
#define MESSAGES_MQTTSNGWINFO_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNGwInfo : public MqttSNBase
{
    private:
        uint8_t gwId = 0;
        uint32_t gwAdd = 0;
        uint16_t gwPort = 0;

    public:
        MqttSNGwInfo();

        void setGwId(uint8_t gatewayId);
        uint8_t getGwId() const;

        void setGwAdd(std::string gatewayAddress);
        std::string getGwAdd() const;

        void setGwPort(uint16_t gatewayPort);
        uint16_t getGwPort() const;

        ~MqttSNGwInfo() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNGWINFO_H_ */
