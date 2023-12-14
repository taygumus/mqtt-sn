#ifndef TYPES_SERVER_TOPICINFO_H_
#define TYPES_SERVER_TOPICINFO_H_

struct TopicInfo {
    TopicIdType topicIdTypeFlag = TopicIdType::NORMAL_TOPIC_ID;
    uint16_t topicId = 0;
};

#endif /* TYPES_SERVER_TOPICINFO_H_ */
