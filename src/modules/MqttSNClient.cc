#include "MqttSNClient.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "types/MsgType.h"
#include "messages/MqttSNBase.h"
#include "messages/MqttSNAdvertise.h"

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
    EV << "Client received packet: " << inet::UdpSocket::getReceivedPacketInfo(pk) << std::endl;

    const auto& header = pk->peekData<MqttSNBase>();
    checkPacketIntegrity((inet::B) pk->getByteLength(), (inet::B) header->getLength());

    switch(header->getMsgType()) {
        case MsgType::ADVERTISE:
            //
            break;

        default:
            throw omnetpp::cRuntimeError("Unknown message type: %d", (uint16_t) header->getMsgType());
    }

    delete pk;
}

MqttSNClient::~MqttSNClient()
{
    //
}

} /* namespace mqttsn */
