#ifndef TYPES_CLIENT_SUBSCRIBER_TOPIC_H_
#define TYPES_CLIENT_SUBSCRIBER_TOPIC_H_

struct Topic {
    std::string topicName;
    TopicIdType topicIdTypeFlag = TopicIdType::NORMAL_TOPIC_ID;
    QoS qosFlag = QoS::QOS_ZERO;
    int subscribeCounter = 0;
    int unsubscribeCounter = 0;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_TOPIC_H_ */
