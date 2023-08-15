#ifndef TYPES_CLIENTINFO_H_
#define TYPES_CLIENTINFO_H_

struct ClientInfo {
    std::string clientId = "";
    std::string willTopic = "";
    std::string willMsg = "";
    uint16_t duration = 0;
    bool dupFlag = false;
    QoS qosFlag = QoS::QOS_ZERO;
    bool retainFlag = false;
    bool willFlag = false;
    bool cleanSessionFlag = false;
    TopicIdType topicIdTypeFlag = TopicIdType::TOPIC_NAME;
};

#endif /* TYPES_CLIENTINFO_H_ */
