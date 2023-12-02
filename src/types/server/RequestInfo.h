#ifndef TYPES_SERVER_REQUESTINFO_H_
#define TYPES_SERVER_REQUESTINFO_H_

struct RequestInfo {
    inet::L3Address subscriberAddress;
    int subscriberPort;
    MsgType messageType;
    uint16_t messagesKey;
};

#endif /* TYPES_SERVER_REQUESTINFO_H_ */
