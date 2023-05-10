#ifndef MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_
#define MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_

#include "MqttSNMsgIdWithTopicId.h"
#include "types/ReturnCode.h"

namespace mqttsn {

class MqttSNMsgIdWithTopicIdPlus : public MqttSNMsgIdWithTopicId
{
    private:
        ReturnCode returnCode;

    public:
        MqttSNMsgIdWithTopicIdPlus();

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode() const;

        ~MqttSNMsgIdWithTopicIdPlus() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMSGIDWITHTOPICIDPLUS_H_ */
