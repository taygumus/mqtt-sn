#ifndef TYPES_GATEWAYINFO_H_
#define TYPES_GATEWAYINFO_H_

struct GatewayInfo {
    inet::L3Address address;
    int port;
    uint16_t duration;
    inet::clocktime_t lastUpdatedTime;
};

#endif /* TYPES_GATEWAYINFO_H_ */
