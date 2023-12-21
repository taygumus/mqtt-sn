#ifndef TYPES_CLIENT_SUBSCRIBER_MESSAGEINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_MESSAGEINFO_H_

struct MessageInfo {
    std::string topicName = "";
    uint16_t topicId = 0;
    bool dup = false;
    QoS qos = QoS::QOS_ZERO;
    bool retain = false;
    std::string data = "";
};

#endif /* TYPES_CLIENT_SUBSCRIBER_MESSAGEINFO_H_ */
