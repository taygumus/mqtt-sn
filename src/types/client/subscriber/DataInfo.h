#ifndef TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_

struct DataInfo {
    std::string topicName = "";
    uint16_t topicId = 0;
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    bool retain = false;
    std::string data = "";
};

#endif /* TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_ */
