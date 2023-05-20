#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        virtual void processStart();
        virtual void processSend();
        virtual void processStop();
        virtual void processPacket(inet::Packet *msg);

    public:
        MqttSNClient() {};
        ~MqttSNClient() {};
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
