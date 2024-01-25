#ifndef TYPES_SERVER_DATAINFO_H_
#define TYPES_SERVER_DATAINFO_H_

struct DataInfo {
    uint16_t topicId = 0;
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    bool retain = false;
    std::string data = "";
    TagInfo tagInfo;
};

#endif /* TYPES_SERVER_DATAINFO_H_ */
