#ifndef TYPES_CLIENTSTATE_H_
#define TYPES_CLIENTSTATE_H_

enum ClientState {
    DISCONNECTED,
    ACTIVE,
    LOST,
    ASLEEP,
    AWAKE,
    INVALID_STATE
};

#endif /* TYPES_CLIENTSTATE_H_ */
