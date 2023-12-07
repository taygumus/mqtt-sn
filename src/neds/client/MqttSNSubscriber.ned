package mqttsn.neds.client;

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        string topicsJson; // json string with topic names and their associated QoS
        
        double subscriptionInterval @unit(s) = default(30s); // subscription interval for new topics
        double unsubscriptionInterval @unit(s) = default(60s); // unsubscription interval from topics
        
        @display("i=block/routing;bgb=38,25");
        @class(MqttSNSubscriber);
}
