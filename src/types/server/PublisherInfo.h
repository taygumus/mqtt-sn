#ifndef TYPES_SERVER_PUBLISHERINFO_H_
#define TYPES_SERVER_PUBLISHERINFO_H_

struct PublisherInfo {
    bool willFlag = false;
    QoS willQoS = QoS::QOS_ZERO;
    bool willRetain = false;
    std::string willTopic = "";
    std::string willMsg = "";
    std::set<uint16_t> messageIds;
};

#endif /* TYPES_SERVER_PUBLISHERINFO_H_ */
