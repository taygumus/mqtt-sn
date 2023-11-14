#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"

namespace mqttsn {

Define_Module(MqttSNSubscriber);

using json = nlohmann::json;

void MqttSNSubscriber::initializeCustom()
{
    fillTopics();
}

bool MqttSNSubscriber::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    // TO DO
    return false;
}

void MqttSNSubscriber::scheduleActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNSubscriber::cancelActiveStateEventsCustom()
{
    // TO DO
}

void MqttSNSubscriber::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    // TO DO
}

void MqttSNSubscriber::processConnAckCustom()
{
    // TO DO
}

void MqttSNSubscriber::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSessionFlag"), MqttSNClient::keepAlive);
}

void MqttSNSubscriber::fillTopics()
{
    json jsonData = json::parse(par("topicsJson").stringValue());
    int topicsKey = 0;

    // iterate over json object keys (topics) and fill the data structure
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        Topic topic;
        topic.topicName = it.key();
        topic.qosFlag = ConversionHelper::intToQoS(it.value()["qos"]);

        topics[topicsKey++] = topic;
    }
}

void MqttSNSubscriber::handleRetransmissionEventCustom(const inet::L3Address& destAddress, const int& destPort,
                                                       omnetpp::cMessage* msg, MsgType msgType)
{
    // TO DO
    switch (msgType) {
        //
        default:
            break;
    }
}

void MqttSNSubscriber::cancelActiveStateClockEventsCustom()
{
    // TO DO
}

MqttSNSubscriber::~MqttSNSubscriber()
{
    // TO DO
}

} /* namespace mqttsn */
