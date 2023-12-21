#ifndef TYPES_CLIENT_PUBLISHER_TOPICINFO_H_
#define TYPES_CLIENT_PUBLISHER_TOPICINFO_H_

struct TopicInfo {
    std::string topicName = "";
    ItemInfo* itemInfo = nullptr;
};

#endif /* TYPES_CLIENT_PUBLISHER_TOPICINFO_H_ */
