#ifndef TYPES_SERVER_SUBSCRIBERTOPICINFO_H_
#define TYPES_SERVER_SUBSCRIBERTOPICINFO_H_

struct SubscriberTopicInfo {
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    bool isRegistered = false;
};

#endif /* TYPES_SERVER_SUBSCRIBERTOPICINFO_H_ */
