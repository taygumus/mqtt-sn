#ifndef TYPES_CLIENT_GATEWAYINFO_H_
#define TYPES_CLIENT_GATEWAYINFO_H_

struct GatewayInfo {
    inet::L3Address address;
    int port = 0;
    uint16_t duration = 0;
    inet::clocktime_t lastUpdatedTime = 0;
};

#endif /* TYPES_CLIENT_GATEWAYINFO_H_ */
