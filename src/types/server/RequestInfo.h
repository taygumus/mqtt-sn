#ifndef TYPES_SERVER_REQUESTINFO_H_
#define TYPES_SERVER_REQUESTINFO_H_

struct RequestInfo {
    inet::clocktime_t requestTime = 0;
    int retransmissionCounter = 0;
    inet::L3Address subscriberAddress;
    int subscriberPort;
    MsgType messageType;
    uint16_t messagesKey = 0;
    uint16_t retainMessagesKey = 0;
};

#endif /* TYPES_SERVER_REQUESTINFO_H_ */
