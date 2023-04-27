#ifndef MESSAGES_MQTTSNCONNECT_H_
#define MESSAGES_MQTTSNCONNECT_H_

#include "MqttSNMessage.h"

namespace mqttsn {

class MqttSNConnect : public MqttSNMessage
{
    private:
        uint8_t flags = 0;
        uint8_t protocolId = 0x01;
        uint16_t duration = 0;
        std::string clientId;

    public:
        MqttSNConnect() {};

        void setWillFlag(bool willFlag);
        bool getWillFlag();

        void setCleanSessionFlag(bool cleanSessionFlag);
        bool getCleanSessionFlag();

        uint8_t getProtocolId();

        void setDuration(uint16_t seconds);
        uint16_t getDuration();

        void setClientId(std::string id);
        std::string getClientId();

        ~MqttSNConnect() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNCONNECT_H_ */
