package mqttsn.neds.client;

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        @class(MqttSNSubscriber);
        @display("i=block/routing;bgb=20,20");
        
        double subscriptionInterval @unit(s) = default(30s); // subscription interval for new topics
        double unsubscriptionInterval @unit(s) = default(60s); // unsubscription interval from topics
}
