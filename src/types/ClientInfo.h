#ifndef TYPES_CLIENTINFO_H_
#define TYPES_CLIENTINFO_H_

struct ClientInfo {
    std::string clientId = "";
    bool dupFlag = false;
    QoS qosFlag = QoS::QOS_ZERO;
    bool retainFlag = false;
    bool willFlag = false;
    bool cleanSessionFlag = false;
    TopicIdType topicIdTypeFlag = TopicIdType::TOPIC_NAME;
    std::string willTopic = "";
    std::string willMsg = "";
};

#endif /* TYPES_CLIENTINFO_H_ */
