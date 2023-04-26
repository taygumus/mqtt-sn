#ifndef MESSAGES_MQTTSNSEARCHGW_H_
#define MESSAGES_MQTTSNSEARCHGW_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNSearchGw : public MqttSNMessage
{
    private:
        uint8_t radius = 0;

    public:
        MqttSNSearchGw() {};

        void setRadius(uint8_t hops);
        uint8_t getRadius();

        ~MqttSNSearchGw() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNSEARCHGW_H_ */
