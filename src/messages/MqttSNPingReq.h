#ifndef MESSAGES_MQTTSNPINGREQ_H_
#define MESSAGES_MQTTSNPINGREQ_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNPingReq : public MqttSNBase
{
    private:
        std::string clientId;

    public:
        MqttSNPingReq() {};

        void setClientId(const std::string& id);
        std::string getClientId() const;

        ~MqttSNPingReq() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNPINGREQ_H_ */
