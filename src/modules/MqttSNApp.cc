#include "MqttSNApp.h"

namespace mqttsn {

void MqttSNApp::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL)
        selfMsg = new inet::ClockEvent("internalTimer");
}

void MqttSNApp::handleMessageWhenUp(omnetpp::cMessage *msg)
{
    if (msg->isSelfMessage()) {

        switch (selfMsg->getKind()) {
            case START:
                processStart();
                break;

            case SEND:
                processSend();
                break;

            case STOP:
                processStop();
                break;

            default:
                throw omnetpp::cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void MqttSNApp::finish()
{
    inet::ApplicationBase::finish();
}

void MqttSNApp::refreshDisplay() const
{
    inet::ApplicationBase::refreshDisplay();
}

void MqttSNApp::processStop()
{
    socket.close();
}

void MqttSNApp::handleStartOperation(inet::LifecycleOperation *operation)
{
    selfMsg->setKind(START);
    scheduleClockEventAt(getClockTime(), selfMsg);
}

void MqttSNApp::handleStopOperation(inet::LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
}

void MqttSNApp::handleCrashOperation(inet::LifecycleOperation *operation)
{
    cancelClockEvent(selfMsg);
    socket.destroy();
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

MqttSNApp::~MqttSNApp()
{
    cancelAndDelete(selfMsg);
}

} /* namespace mqttsn */
