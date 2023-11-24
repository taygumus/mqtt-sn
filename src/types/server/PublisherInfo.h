#ifndef TYPES_SERVER_PUBLISHERINFO_H_
#define TYPES_SERVER_PUBLISHERINFO_H_

struct PublisherInfo {
    bool willFlag = false;
    QoS willQoSFlag = QoS::QOS_ZERO;
    bool willRetainFlag = false;
    std::string willTopic = "";
    std::string willMsg = "";
    std::set<uint16_t> messageIds;
};

#endif /* TYPES_SERVER_PUBLISHERINFO_H_ */
