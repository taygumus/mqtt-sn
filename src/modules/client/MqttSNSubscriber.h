#ifndef MODULES_CLIENT_MQTTSNSUBSCRIBER_H_
#define MODULES_CLIENT_MQTTSNSUBSCRIBER_H_

#include "MqttSNClient.h"

namespace mqttsn {

class MqttSNSubscriber : public MqttSNClient {

    public:
        MqttSNSubscriber();
        ~MqttSNSubscriber();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNSUBSCRIBER_H_ */
