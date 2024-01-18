#ifndef TYPES_CLIENT_PUBLISHER_ITEMINFO_H_
#define TYPES_CLIENT_PUBLISHER_ITEMINFO_H_

struct ItemInfo {
    std::string topicName = "";
    uint16_t topicId = 0; // used only as predefined topic ID
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    int counter = 0;
    std::map<int, DataInfo> data;
};

#endif /* TYPES_CLIENT_PUBLISHER_ITEMINFO_H_ */
