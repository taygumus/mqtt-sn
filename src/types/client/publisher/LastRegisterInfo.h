#ifndef TYPES_CLIENT_PUBLISHER_LASTREGISTERINFO_H_
#define TYPES_CLIENT_PUBLISHER_LASTREGISTERINFO_H_

struct LastRegisterInfo {
    RegisterInfo info;
    bool retry = false;
};

#endif /* TYPES_CLIENT_PUBLISHER_LASTREGISTERINFO_H_ */
