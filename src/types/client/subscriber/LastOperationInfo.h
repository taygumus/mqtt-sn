#ifndef TYPES_CLIENT_SUBSCRIBER_LASTOPERATIONINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_LASTOPERATIONINFO_H_

struct LastOperationInfo {
    std::string topicName = "";
    ItemInfo* itemInfo = nullptr;
    bool retry = false;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_LASTOPERATIONINFO_H_ */
