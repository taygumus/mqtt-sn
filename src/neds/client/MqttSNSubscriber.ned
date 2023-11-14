package mqttsn.neds.client;

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        string topicsJson; // json string with topic names and their associated QoS
        
        @display("i=block/routing;bgb=38,25");
        @class(MqttSNSubscriber);
}
