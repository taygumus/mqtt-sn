#ifndef MESSAGES_MQTTSNBASEWITHMSGID_H_
#define MESSAGES_MQTTSNBASEWITHMSGID_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNBaseWithMsgId : public MqttSNBase
{
    private:
        uint16_t msgId = 0;

    public:
        MqttSNBaseWithMsgId();

        void setMsgId(uint16_t messageId);
        uint16_t getMsgId() const;

        ~MqttSNBaseWithMsgId() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHMSGID_H_ */
