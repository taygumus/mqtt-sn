#ifndef MESSAGES_MQTTSNDISCONNECT_H_
#define MESSAGES_MQTTSNDISCONNECT_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNDisconnect : public MqttSNBase
{
    private:
        uint16_t duration = 0;

    public:
        MqttSNDisconnect() {};

        void setDuration(uint16_t seconds);
        uint16_t getDuration() const;

        ~MqttSNDisconnect() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNDISCONNECT_H_ */
