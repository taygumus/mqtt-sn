#ifndef TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_
#define TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_

struct LastPublishInfo {
    uint16_t topicId = 0;
    RegisterInfo registerInfo;
    DataInfo dataInfo;
    bool retry = false;
};

#endif /* TYPES_CLIENT_PUBLISHER_LASTPUBLISHINFO_H_ */
