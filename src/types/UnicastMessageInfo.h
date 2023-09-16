#ifndef TYPES_UNICASTMESSAGEINFO_H_
#define TYPES_UNICASTMESSAGEINFO_H_

struct UnicastMessageInfo {
    std::string name;
    inet::ClockEvent *retransmissionEvent = nullptr;
    int retransmissionCounter = 0;
    inet::L3Address destAddress;
    int destPort;
};

#endif /* TYPES_UNICASTMESSAGEINFO_H_ */
