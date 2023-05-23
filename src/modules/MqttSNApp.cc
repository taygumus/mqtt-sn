#include "MqttSNApp.h"

namespace mqttsn {

void MqttSNApp::finish()
{
    inet::ApplicationBase::finish();
}

void MqttSNApp::refreshDisplay() const
{
    inet::ApplicationBase::refreshDisplay();
}

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

} /* namespace mqttsn */
