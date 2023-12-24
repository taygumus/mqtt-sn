#ifndef TYPES_SERVER_SUBSCRIBERINFO_H_
#define TYPES_SERVER_SUBSCRIBERINFO_H_

struct SubscriberInfo {
    std::set<uint16_t> topicIds;
};

#endif /* TYPES_SERVER_SUBSCRIBERINFO_H_ */
