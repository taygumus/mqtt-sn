#include "MqttSNClient.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::sendPacket()
{
    EV << "Client Works! \n";
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    //
}

} /* namespace mqttsn */
