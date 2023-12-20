package mqttsn.neds.client;

module MqttSNPublisher extends MqttSNClient
{
    parameters:
        @class(MqttSNPublisher);
        @display("i=block/routing;bgb=20,20");
        
        bool will = default(false);
        int willQoS = default(0); // valid QoS values: 0, 1, 2 and -1
        bool willRetain = default(false);
        string willTopic = default(""); // will topic name
        string willMsg = default(""); // will message
        
        double registrationInterval @unit(s) = default(5s); // registration interval for new topic names
        double publishInterval @unit(s) = default(10s); // publish interval for new messages
}
