#ifndef TYPES_SERVER_REGISTERINFO_H_
#define TYPES_SERVER_REGISTERINFO_H_

struct RegisterInfo {
    inet::clocktime_t requestTime = 0;
    int retransmissionCounter = 0;
    inet::L3Address subscriberAddress;
    int subscriberPort = 0;
    std::string topicName = "";
    uint16_t topicId = 0;
};

#endif /* TYPES_SERVER_REGISTERINFO_H_ */
