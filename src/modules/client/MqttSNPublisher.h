#ifndef MODULES_CLIENT_MQTTSNPUBLISHER_H_
#define MODULES_CLIENT_MQTTSNPUBLISHER_H_

#include "MqttSNClient.h"

namespace mqttsn {

class MqttSNPublisher : public MqttSNClient {

    public:
        MqttSNPublisher();
        ~MqttSNPublisher();
};

} /* namespace mqttsn */

#endif /* MODULES_CLIENT_MQTTSNPUBLISHER_H_ */
