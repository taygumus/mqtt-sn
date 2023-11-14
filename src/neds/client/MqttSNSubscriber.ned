package mqttsn.neds.client;

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        string topicsJson; // json string with topic names and their associated QoS
        
        double subscriptionInterval @unit(s) = default(5s); // subscription interval for new topics
        
        @display("i=block/routing;bgb=38,25");
        @class(MqttSNSubscriber);
}
