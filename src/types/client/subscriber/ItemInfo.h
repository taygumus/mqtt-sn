#ifndef TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_

struct ItemInfo {
    std::string topicName = "";
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    uint16_t topicId = 0; // predefined topic ID
    QoS qos = QoS::QOS_ZERO;
    int subscribeCounter = 0;
    int unsubscribeCounter = 0;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_ */
