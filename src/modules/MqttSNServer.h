#ifndef MODULES_MQTTSNSERVER_H_
#define MODULES_MQTTSNSERVER_H_

#include "inet/applications/udpapp/UdpBasicApp.h"

namespace mqttsn {

class MqttSNServer : public inet::UdpBasicApp
{
    protected:
        virtual void sendPacket() override;
        virtual void processPacket(inet::Packet *msg) override;
    public:
        MqttSNServer() {};
        ~MqttSNServer() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNSERVER_H_ */
