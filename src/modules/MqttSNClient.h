#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "inet/applications/udpapp/UdpBasicApp.h"

namespace mqttsn {

class MqttSNClient : public inet::UdpBasicApp
{
    protected:
        virtual void sendPacket() override;
        virtual void processPacket(inet::Packet *msg) override;
    public:
        MqttSNClient() {};
        ~MqttSNClient() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
