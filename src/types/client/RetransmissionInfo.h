#ifndef TYPES_CLIENT_RETRANSMISSIONINFO_H_
#define TYPES_CLIENT_RETRANSMISSIONINFO_H_

struct RetransmissionInfo {
    inet::ClockEvent *retransmissionEvent = nullptr;
    int retransmissionCounter = 0;
    inet::L3Address destAddress;
    int destPort;
};

#endif /* TYPES_CLIENT_RETRANSMISSIONINFO_H_ */
