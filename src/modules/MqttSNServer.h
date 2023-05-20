#ifndef MODULES_MQTTSNSERVER_H_
#define MODULES_MQTTSNSERVER_H_

#include "MqttSNApp.h"

namespace mqttsn {

class MqttSNServer : public MqttSNApp
{
    protected:
        virtual void processStart();
        virtual void processSend();
        virtual void processStop();
        virtual void processPacket(inet::Packet *msg);

    public:
        MqttSNServer() {};
        ~MqttSNServer() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNSERVER_H_ */
