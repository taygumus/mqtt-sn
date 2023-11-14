#include "MqttSNSubscriber.h"
#include "externals/nlohmann/json.hpp"
#include "helpers/ConversionHelper.h"

namespace mqttsn {

Define_Module(MqttSNSubscriber);

using json = nlohmann::json;

void MqttSNSubscriber::initializeCustom()
{
    fillTopics();

    subscriptionInterval = par("subscriptionInterval");
    subscriptionEvent = new inet::ClockEvent("subscriptionTimer");
}

bool MqttSNSubscriber::handleMessageWhenUpCustom(omnetpp::cMessage* msg)
{
    if (msg == subscriptionEvent) {
        handleSubscriptionEvent();
    }
    else {
        return false;
    }

    return true;
}

void MqttSNSubscriber::scheduleActiveStateEventsCustom()
{
    //
}

void MqttSNSubscriber::cancelActiveStateEventsCustom()
{
    cancelEvent(subscriptionEvent);
}

void MqttSNSubscriber::cancelActiveStateClockEventsCustom()
{
    cancelClockEvent(subscriptionEvent);
}

void MqttSNSubscriber::processPacketCustom(inet::Packet* pk, const inet::L3Address& srcAddress, const int& srcPort, MsgType msgType)
{
    // TO DO
}

void MqttSNSubscriber::processConnAckCustom()
{
    scheduleClockEventAfter(subscriptionInterval, subscriptionEvent);
}

void MqttSNSubscriber::handleCheckConnectionEventCustom(const inet::L3Address& destAddress, const int& destPort)
{
    MqttSNClient::sendConnect(destAddress, destPort, 0, par("cleanSessionFlag"), MqttSNClient::keepAlive);
}

void MqttSNSubscriber::handleSubscriptionEvent()
{
    // TO DO
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

MqttSNSubscriber::~MqttSNSubscriber()
{
    cancelAndDelete(subscriptionEvent);
}

} /* namespace mqttsn */
