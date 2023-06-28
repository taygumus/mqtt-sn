#ifndef MODULES_MQTTSNCLIENT_H_
#define MODULES_MQTTSNCLIENT_H_

#include "MqttSNApp.h"
#include "types/GatewayInfo.h"

namespace mqttsn {

class MqttSNClient : public MqttSNApp
{
    protected:
        // parameters
        double checkGatewaysInterval;
        uint16_t temporaryDuration;

        // state
        inet::ClockEvent *checkGatewaysEvent = nullptr;
        std::map<uint8_t, GatewayInfo> activeGateways;

        inet::ClockEvent *searchGatewayEvent = nullptr;
        double searchGatewayInterval;
        bool maxIntervalReached = false;
        bool searchGateway = true;

        inet::ClockEvent *gatewayInfoEvent = nullptr;
        double gatewayInfoInterval;

    protected:
        virtual void initialize(int stage) override;
        virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
        virtual void finish() override;
        virtual void refreshDisplay() const override;

        virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
        virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
        virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

        // process received packets
        virtual void processPacket(inet::Packet *pk) override;
        virtual void processAdvertise(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);
        virtual void processSearchGw(inet::Packet *pk);
        virtual void processGwInfo(inet::Packet *pk, inet::L3Address srcAddress, int srcPort);

        // send packets
        virtual void sendSearchGw();

        // event handlers
        virtual void handleCheckGatewaysEvent();
        virtual void handleSearchGatewayEvent();

        // others
        virtual void checkGatewaysAvailability();
        virtual void updateActiveGateways(uint8_t gatewayId, uint16_t duration, inet::L3Address srcAddress, int srcPort);

    public:
        MqttSNClient() {};
        ~MqttSNClient();
};

} /* namespace mqttsn */

#endif /* MODULES_MQTTSNCLIENT_H_ */
