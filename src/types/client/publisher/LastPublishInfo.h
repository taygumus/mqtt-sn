#ifndef TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_
#define TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_

struct LastPublishInfo {
    std::string topicName = "";
    uint16_t topicId = 0;
    ItemInfo* itemInfo = nullptr;
    DataInfo* dataInfo = nullptr;
    TagInfo tagInfo;
    bool retry = false;
};

#endif /* TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_ */
