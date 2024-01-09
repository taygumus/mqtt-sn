package mqttsn.neds.client;

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        @class(MqttSNSubscriber);
        @display("i=block/routing;bgb=20,20");
        
        double subscriptionInterval @unit(s) = default(5s); // subscription interval for new topics
        int subscriptionLimit = default(-1); // maximum subscriptions, -1 for unlimited
        
        double unsubscriptionInterval @unit(s) = default(20s); // unsubscription interval from topics
        int unsubscriptionLimit = default(-1); // maximum unsubscriptions, -1 for unlimited
}
