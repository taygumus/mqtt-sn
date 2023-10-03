#ifndef TYPES_SERVER_CLIENTINFO_H_
#define TYPES_SERVER_CLIENTINFO_H_

struct ClientInfo {
    bool isNew = true;
    std::string clientId = "";
    uint16_t keepAliveDuration = 0;
    uint16_t sleepDuration = 0;
    ClientState currentState = ClientState::DISCONNECTED;
    inet::clocktime_t lastReceivedMsgTime = 0;
    bool sentPingReq = false;
};

#endif /* TYPES_SERVER_CLIENTINFO_H_ */
