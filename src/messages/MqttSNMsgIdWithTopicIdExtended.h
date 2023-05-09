#ifndef MESSAGES_MQTTSNMSGIDWITHTOPICIDEXTENDED_H_
#define MESSAGES_MQTTSNMSGIDWITHTOPICIDEXTENDED_H_

#include "MqttSNMsgIdWithTopicId.h"
#include "types/ReturnCode.h"

namespace mqttsn {

class MqttSNMsgIdWithTopicIdExtended : public MqttSNMsgIdWithTopicId
{
    private:
        ReturnCode returnCode;

    public:
        MqttSNMsgIdWithTopicIdExtended();

        void setReturnCode(ReturnCode code);
        ReturnCode getReturnCode() const;

        ~MqttSNMsgIdWithTopicIdExtended() {};
};

} /* namespace mqttsn */

#endif /* MESSAGES_MQTTSNMSGIDWITHTOPICIDEXTENDED_H_ */
