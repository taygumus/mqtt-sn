#include "MqttSNPublisher.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"

namespace mqttsn {

Define_Module(MqttSNPublisher);

void MqttSNPublisher::initializeCustom()
{
    // TO DO
}

bool MqttSNPublisher::handleMessageWhenUpCustom(omnetpp::cMessage *msg)
{
    // TO DO
    return false;
}

void MqttSNPublisher::scheduleActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNPublisher::cancelActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNPublisher::cancelActiveStateClockEventsCustom()
{
    // TO DO
}

void MqttSNPublisher::processPacketCustom(MsgType msgType, inet::Packet *pk, inet::L3Address srcAddress, int srcPort)
{
    switch(msgType) {
        // packet types that are allowed only from the selected gateway
        case MsgType::WILLTOPICREQ:
        case MsgType::WILLMSGREQ:
            if (!isSelectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

        // packet types that are allowed only from the connected gateway
        // TO DO

        default:
            break;
    }

    switch(msgType) {
        case MsgType::WILLTOPICREQ:
            processWillTopicReq(srcAddress, srcPort);
            break;

        case MsgType::WILLMSGREQ:
            processWillMsgReq(srcAddress, srcPort);
            break;

        default:
            break;
    }
}

void MqttSNPublisher::processWillTopicReq(inet::L3Address srcAddress, int srcPort)
{
    sendBaseWithWillTopic(srcAddress, srcPort, MsgType::WILLTOPIC, MqttSNClient::intToQoS(par("qosFlag")), par("retainFlag"), par("willTopic"));
}

void MqttSNPublisher::processWillMsgReq(inet::L3Address srcAddress, int srcPort)
{
    sendBaseWithWillMsg(srcAddress, srcPort, MsgType::WILLMSG, par("willMsg"));
}

void MqttSNPublisher::sendBaseWithWillTopic(inet::L3Address destAddress, int destPort, MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillTopic>();
    payload->setMsgType(msgType);
    payload->setQoSFlag(qosFlag);
    payload->setRetainFlag(retainFlag);
    payload->setWillTopic(willTopic);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::WILLTOPIC:
            packetName = "WillTopicPacket";
            break;

        case MsgType::WILLTOPICUPD:
            packetName = "WillTopicUpdPacket";
            break;

        default:
            packetName = "BaseWithWillTopicPacket";
    }

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendBaseWithWillMsg(inet::L3Address destAddress, int destPort, MsgType msgType, std::string willMsg)
{
    const auto& payload = inet::makeShared<MqttSNBaseWithWillMsg>();
    payload->setMsgType(msgType);
    payload->setWillMsg(willMsg);
    payload->setChunkLength(inet::B(payload->getLength()));

    std::string packetName;

    switch(msgType) {
        case MsgType::WILLMSG:
            packetName = "WillMsgPacket";
            break;

        case MsgType::WILLMSGUPD:
            packetName = "WillMsgUpdPacket";
            break;

        default:
            packetName = "BaseWithWillMsgPacket";
    }

    inet::Packet *packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::handleCheckConnectionEventCustom(inet::L3Address destAddress, int destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, par("willFlag"), 0, keepAlive);
}

void MqttSNPublisher::handleRetransmissionEventCustom(MsgType msgType, inet::L3Address destAddress, int destPort, omnetpp::cMessage *msg, bool retransmission)
{
    // TO DO
    switch (msgType) {
        //
        default:
            break;
    }
}

MqttSNPublisher::~MqttSNPublisher()
{
    // TO DO
}

} /* namespace mqttsn */
