package mqttsn.neds.client;

module MqttSNPublisher extends MqttSNClient
{
    parameters:
        bool willFlag = default(false);
        int willQosFlag = default(0); // valid qos values: 0, 1, 2 and -1
        bool willRetainFlag = default(false);
        string willTopic = default(""); // will topic name
        string willMsg = default(""); // will message
        
        @display("i=block/routing;bgb=38,25");
        @class(MqttSNPublisher);
}
