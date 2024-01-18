#ifndef TYPES_SERVER_RETAINMESSAGEINFO_H_
#define TYPES_SERVER_RETAINMESSAGEINFO_H_

struct RetainMessageInfo {
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    bool dup = false;
    QoS qos = QoS::QOS_ZERO;
    std::string data = "";
};

#endif /* TYPES_SERVER_RETAINMESSAGEINFO_H_ */
