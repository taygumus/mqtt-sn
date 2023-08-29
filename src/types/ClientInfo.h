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
    ClientState currentState = ClientState::DISCONNECTED;
    inet::clocktime_t lastReceivedMsgTime = 0;
    bool sentPingReq = false;
};

#endif /* TYPES_CLIENTINFO_H_ */
