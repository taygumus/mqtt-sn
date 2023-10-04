#include "MqttSNPublisher.h"
#include "messages/MqttSNBaseWithWillTopic.h"
#include "messages/MqttSNBaseWithWillMsg.h"
#include "messages/MqttSNBaseWithReturnCode.h"

namespace mqttsn {

Define_Module(MqttSNPublisher);

void MqttSNPublisher::initializeCustom()
{
    willQosFlag = par("willQosFlag");
    willRetainFlag = par("willRetainFlag");
    willTopic = par("willTopic").stringValue();
    willMsg = par("willMsg").stringValue();
}

bool MqttSNPublisher::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
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

void MqttSNPublisher::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    switch(msgType) {
        // packet types that are allowed only from the selected gateway
        case MsgType::WILLTOPICREQ:
        case MsgType::WILLMSGREQ:
            if (!MqttSNClient::isSelectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

        // packet types that are allowed only from the connected gateway
        case MsgType::WILLTOPICRESP:
        case MsgType::WILLMSGRESP:
            if (!MqttSNClient::isConnectedGateway(srcAddress, srcPort)) {
                return;
            }
            break;

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

        case MsgType::WILLTOPICRESP:
            processWillResp(pk, true);
            break;

        case MsgType::WILLMSGRESP:
            processWillResp(pk, false);
            break;

        default:
            break;
    }
}

void MqttSNPublisher::processWillTopicReq(const inet::L3Address& srcAddress, const int& srcPort)
{
    sendBaseWithWillTopic(srcAddress, srcPort, MsgType::WILLTOPIC, MqttSNClient::intToQoS(willQosFlag), willRetainFlag, willTopic);
}

void MqttSNPublisher::processWillMsgReq(const inet::L3Address& srcAddress, const int& srcPort)
{
    sendBaseWithWillMsg(srcAddress, srcPort, MsgType::WILLMSG, willMsg);
}

void MqttSNPublisher::processWillResp(inet::Packet* pk, bool willTopic)
{
    const auto& payload = pk->peekData<MqttSNBaseWithReturnCode>();

    if (payload->getReturnCode() != ReturnCode::ACCEPTED) {
        return;
    }

    if (willTopic) {
        EV << "Will topic name updated" << std::endl;
        return;
    }

    EV << "Will message updated" << std::endl;
}

void MqttSNPublisher::sendBaseWithWillTopic(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, QoS qosFlag, bool retainFlag, std::string willTopic)
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

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::sendBaseWithWillMsg(const inet::L3Address& destAddress, const int& destPort, MsgType msgType, std::string willMsg)
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

    inet::Packet* packet = new inet::Packet(packetName.c_str());
    packet->insertAtBack(payload);

    socket.sendTo(packet, destAddress, destPort);
}

void MqttSNPublisher::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, par("willFlag"), par("cleanSessionFlag"), MqttSNClient::keepAlive);
}

void MqttSNPublisher::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort, omnetpp::cMessage* msg, MsgType msgType, bool retransmission)
{
    switch (msgType) {
        case MsgType::WILLTOPICUPD:
            retransmitWillTopicUpd(destAddress, destPort, retransmission);
            break;

        case MsgType::WILLMSGUPD:
            retransmitWillMsgUpd(destAddress, destPort, retransmission);
            break;

        default:
            break;
    }
}

void MqttSNPublisher::retransmitWillTopicUpd(const inet::L3Address& destAddress, const int& destPort, bool retransmission)
{
    if (!retransmission) {
        return;
    }

    sendBaseWithWillTopic(destAddress, destPort, MsgType::WILLTOPICUPD, MqttSNClient::intToQoS(willQosFlag), willRetainFlag, willTopic);
}

void MqttSNPublisher::retransmitWillMsgUpd(const inet::L3Address& destAddress, const int& destPort, bool retransmission)
{
    if (!retransmission) {
        return;
    }

    sendBaseWithWillMsg(destAddress, destPort, MsgType::WILLMSGUPD, willMsg);
}

MqttSNPublisher::~MqttSNPublisher()
{
    // TO DO
}

} /* namespace mqttsn */
