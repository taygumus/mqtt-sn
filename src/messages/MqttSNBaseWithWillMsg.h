#ifndef MESSAGES_MQTTSNBASEWITHWILLMSG_H_
#define MESSAGES_MQTTSNBASEWITHWILLMSG_H_

#include "MqttSNBase.h"

namespace mqttsn {

class MqttSNBaseWithWillMsg : public MqttSNBase
{
    private:
        std::string willMsg;

    public:
        MqttSNBaseWithWillMsg() {};

        void setWillMsg(const std::string& willMessage);
        std::string getWillMsg() const;

        ~MqttSNBaseWithWillMsg() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHWILLMSG_H_ */
