#ifndef TYPES_SERVER_MESSAGEINFO_H_
#define TYPES_SERVER_MESSAGEINFO_H_

struct MessageInfo {
    bool dup = false;
    QoS qos = QoS::QOS_ZERO;
    bool retain = false;
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    uint16_t topicId = 0;
    std::string data;
};

#endif /* TYPES_SERVER_MESSAGEINFO_H_ */
