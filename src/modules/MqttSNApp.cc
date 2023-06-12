#include "MqttSNApp.h"

namespace mqttsn {

void MqttSNApp::socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet)
{
    processPacket(packet);
}

void MqttSNApp::socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << std::endl;
    delete indication;
}

void MqttSNApp::socketClosed(inet::UdpSocket *socket)
{
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(-1);
}

void MqttSNApp::checkPacketIntegrity(inet::B receivedLength, inet::B fieldLength)
{
    if (receivedLength != fieldLength) {
        throw omnetpp::cRuntimeError(
                "Packet integrity error: Received length (%d bytes) does not match the expected length (%d bytes)",
                (uint16_t) receivedLength.get(),
                (uint16_t) fieldLength.get()
        );
    }
}

bool MqttSNApp::isSelfBroadcastAddress(inet::L3Address address)
{
    inet::L3Address selfBroadcastAddress = inet::L3Address("127.0.0.1");
    return (address == selfBroadcastAddress);
}

} /* namespace mqttsn */
