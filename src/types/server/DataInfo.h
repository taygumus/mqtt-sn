#ifndef TYPES_SERVER_DATAINFO_H_
#define TYPES_SERVER_DATAINFO_H_

struct DataInfo {
    bool dupFlag = false;
    QoS qosFlag = QoS::QOS_ZERO;
    uint16_t topicId = 0;
    std::string message;
};

#endif /* TYPES_SERVER_DATAINFO_H_ */
