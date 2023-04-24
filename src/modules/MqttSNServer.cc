#include "MqttSNServer.h"

namespace mqttsn {

Define_Module(MqttSNServer);

void MqttSNServer::sendPacket()
{
    //
}

void MqttSNServer::processPacket(inet::Packet *pk)
{
    EV << "Server has received packet..\n";

    std::stringstream os;
    os << pk;

    EV << os.str();
}

} /* namespace mqttsn */
