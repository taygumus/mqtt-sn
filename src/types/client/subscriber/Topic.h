#ifndef TYPES_CLIENT_SUBSCRIBER_TOPIC_H_
#define TYPES_CLIENT_SUBSCRIBER_TOPIC_H_

struct Topic {
    std::string topicName;
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    uint16_t predefinedTopicId = 0;
    QoS qos = QoS::QOS_ZERO;
    int subscribeCounter = 0;
    int unsubscribeCounter = 0;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_TOPIC_H_ */
