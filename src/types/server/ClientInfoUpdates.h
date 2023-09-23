#ifndef TYPES_SERVER_CLIENTINFOUPDATES_H_
#define TYPES_SERVER_CLIENTINFOUPDATES_H_

struct ClientInfoUpdates {
    bool clientId = false;
    bool willTopic = false;
    bool willMsg = false;
    bool keepAliveDuration = false;
    bool sleepDuration = false;
    bool dupFlag = false;
    bool qosFlag = false;
    bool retainFlag = false;
    bool willFlag = false;
    bool cleanSessionFlag = false;
    bool topicIdTypeFlag = false;
    bool currentState = false;
    bool lastReceivedMsgTime = false;
    bool sentPingReq = false;
};

#endif /* TYPES_SERVER_CLIENTINFOUPDATES_H_ */
