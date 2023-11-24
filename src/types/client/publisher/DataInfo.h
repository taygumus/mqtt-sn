#ifndef TYPES_CLIENT_PUBLISHER_DATAINFO_H_
#define TYPES_CLIENT_PUBLISHER_DATAINFO_H_

struct DataInfo {
    QoS qosFlag = QoS::QOS_ZERO;
    bool retainFlag = false;
    std::string data;
};

#endif /* TYPES_CLIENT_PUBLISHER_DATAINFO_H_ */
