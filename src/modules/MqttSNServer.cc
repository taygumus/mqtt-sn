#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "inet/applications/udpapp/UdpBasicApp.h"

using namespace inet;

namespace mqttsn {

class MqttSNServer : public UdpBasicApp
{
    protected:
         virtual void sendPacket() override;
         virtual void processPacket(Packet *msg) override;
};

Define_Module(MqttSNServer);

void MqttSNServer::sendPacket()
{
    EV << "Server Works!" << endl;
}

void MqttSNServer::processPacket(Packet *pk)
{
    //
}

};
