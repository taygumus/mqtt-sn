#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "inet/applications/udpapp/UdpBasicApp.h"

using namespace inet;

namespace mqttsn {

class MqttSNClient : public UdpBasicApp
{
    protected:
         virtual void sendPacket() override;
         virtual void processPacket(Packet *msg) override;
};

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client Works!" << endl;
}

void MqttSNClient::processPacket(Packet *pk)
{
    //
}

};
