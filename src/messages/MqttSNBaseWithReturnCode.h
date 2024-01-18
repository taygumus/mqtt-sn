#ifndef MESSAGES_MQTTSNBASEWITHRETURNCODE_H_
#define MESSAGES_MQTTSNBASEWITHRETURNCODE_H_

#include "MqttSNBase.h"
#include "types/shared/ReturnCode.h"

namespace mqttsn {

class MqttSNBaseWithReturnCode : public MqttSNBase
{
    private:
        ReturnCode returnCode;

    public:
        MqttSNBaseWithReturnCode();

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode() const;

        ~MqttSNBaseWithReturnCode() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHRETURNCODE_H_ */
