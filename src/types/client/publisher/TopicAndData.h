#ifndef TYPES_CLIENT_PUBLISHER_TOPICANDDATA_H_
#define TYPES_CLIENT_PUBLISHER_TOPICANDDATA_H_

struct TopicAndData {
    std::string topicName;
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    int counter = 0;
    std::map<int, DataInfo> data;
};

#endif /* TYPES_CLIENT_PUBLISHER_TOPICANDDATA_H_ */
