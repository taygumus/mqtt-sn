#ifndef TYPES_CLIENTINFOUPDATES_H_
#define TYPES_CLIENTINFOUPDATES_H_

struct ClientInfoUpdates {
    bool clientId = false;
    bool willTopic = false;
    bool willMsg = false;
    bool duration = false;
    bool dupFlag = false;
    bool qosFlag = false;
    bool retainFlag = false;
    bool willFlag = false;
    bool cleanSessionFlag = false;
    bool topicIdTypeFlag = false;
    bool currentState = false;
    bool lastPingTime = false;
};

#endif /* TYPES_CLIENTINFOUPDATES_H_ */
