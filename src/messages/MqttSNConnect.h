#ifndef MESSAGES_MQTTSNCONNECT_H_
#define MESSAGES_MQTTSNCONNECT_H_

#include "MqttSNBaseWithDuration.h"

namespace mqttsn {

class MqttSNConnect : public MqttSNBaseWithDuration
{
    private:
        uint8_t flags = 0;
        uint8_t protocolId = 0x01;
        std::string clientId;

    public:
        MqttSNConnect();

        void setWillFlag(bool willFlag);
        bool getWillFlag() const;

        void setCleanSessionFlag(bool cleanSessionFlag);
        bool getCleanSessionFlag() const;

        uint8_t getProtocolId() const;

        void setClientId(const std::string& id);
        std::string getClientId() const;

        ~MqttSNConnect() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNCONNECT_H_ */
