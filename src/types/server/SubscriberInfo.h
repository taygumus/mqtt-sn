#ifndef TYPES_SERVER_SUBSCRIBERINFO_H_
#define TYPES_SERVER_SUBSCRIBERINFO_H_

struct SubscriberInfo {
    std::map<uint16_t, SubscriberTopicInfo> subscriberTopics;
};

#endif /* TYPES_SERVER_SUBSCRIBERINFO_H_ */
