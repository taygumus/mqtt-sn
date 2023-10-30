#ifndef MESSAGES_MQTTSNBASEWITHWILLTOPIC_H_
#define MESSAGES_MQTTSNBASEWITHWILLTOPIC_H_

#include "MqttSNBase.h"
#include "types/shared/QoS.h"

namespace mqttsn {

class MqttSNBaseWithWillTopic : public MqttSNBase
{
private:
        uint8_t flags = 0;
        std::string willTopic;

    public:
        MqttSNBaseWithWillTopic();

        void setQoSFlag(QoS qosFlag);
        uint8_t getQoSFlag() const;

        void setRetainFlag(bool retainFlag);
        bool getRetainFlag() const;

        void setWillTopic(const std::string& topicName);
        std::string getWillTopic() const;

        ~MqttSNBaseWithWillTopic() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNBASEWITHWILLTOPIC_H_ */
