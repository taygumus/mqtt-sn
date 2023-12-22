#ifndef TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_

struct ItemInfo {
    std::string topicName = "";
    uint16_t topicId = 0; // used only as predefined topic ID
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    QoS qos = QoS::QOS_ZERO;
    int subscribeCounter = 0;
    int unsubscribeCounter = 0;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_ */
