#ifndef MESSAGES_MQTTSNBASEWITHDURATION_H_
#define MESSAGES_MQTTSNBASEWITHDURATION_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNBaseWithDuration : public MqttSNBase
{
    private:
        uint16_t duration = 0;

    public:
        MqttSNBaseWithDuration();

        void setDuration(uint16_t seconds);
        uint16_t getDuration() const;

        ~MqttSNBaseWithDuration() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHDURATION_H_ */
