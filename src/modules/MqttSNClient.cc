#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace mqttsn {

Define_Module(MqttSNClient);

void MqttSNClient::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        //
    }
}

void MqttSNClient::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    socket.processMessage(msg);
}

void MqttSNClient::finish()
{
    inet::ApplicationBase::finish();
}

void MqttSNClient::refreshDisplay() const
{
    inet::ApplicationBase::refreshDisplay();
}

void MqttSNClient::handleStartOperation(inet::LifecycleOperation *operation)
{
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), par("localPort"));
    socket.setBroadcast(true);

    //
}

void MqttSNClient::handleStopOperation(inet::LifecycleOperation *operation)
{
    socket.close();
}

void MqttSNClient::handleCrashOperation(inet::LifecycleOperation *operation)
{
    socket.destroy();
}

void MqttSNClient::processPacket(inet::Packet *pk)
{
    EV_INFO << "...Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;
    delete pk;
}

void MqttSNClient::sendPacket()
{
    //
}

MqttSNClient::~MqttSNClient()
{
    //
}

} /* namespace mqttsn */
