#ifndef TYPES_CLIENT_PUBLISHER_DATAINFO_H_
#define TYPES_CLIENT_PUBLISHER_DATAINFO_H_

struct DataInfo {
    QoS qos = QoS::QOS_ZERO;
    bool retain = false;
    std::string data;
};

#endif /* TYPES_CLIENT_PUBLISHER_DATAINFO_H_ */
