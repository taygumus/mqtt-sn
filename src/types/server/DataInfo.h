#ifndef TYPES_SERVER_DATAINFO_H_
#define TYPES_SERVER_DATAINFO_H_

struct DataInfo {
    bool retainFlag = false;
    uint16_t topicId = 0;
    std::string data;
};

#endif /* TYPES_SERVER_DATAINFO_H_ */
