#ifndef TYPES_CLIENTINFO_H_
#define TYPES_CLIENTINFO_H_

struct ClientInfo {
    inet::L3Address address;
    int port;
};

#endif /* TYPES_CLIENTINFO_H_ */
