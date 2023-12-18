#ifndef TYPES_SERVER_PUBLISHERINFO_H_
#define TYPES_SERVER_PUBLISHERINFO_H_

struct PublisherInfo {
    bool will = false;
    QoS willQoS = QoS::QOS_ZERO;
    bool willRetain = false;
    std::string willTopic = "";
    std::string willMsg = "";
    std::map<uint16_t, DataInfo> messages;
};

#endif /* TYPES_SERVER_PUBLISHERINFO_H_ */
