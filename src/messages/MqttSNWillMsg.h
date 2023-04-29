#ifndef MESSAGES_MQTTSNWILLMSG_H_
#define MESSAGES_MQTTSNWILLMSG_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNWillMsg : public MqttSNMessage
{
    private:
        std::string willMsg;

    public:
        MqttSNWillMsg() {};

        void setWillMsg(std::string willMessage);
        std::string getWillMsg() const;

        ~MqttSNWillMsg() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNWILLMSG_H_ */
