#ifndef TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_

struct DataInfo {
    bool retainFlag = false;
    uint16_t topicId = 0;
    std::string topicName;
    std::string data;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_DATAINFO_H_ */
