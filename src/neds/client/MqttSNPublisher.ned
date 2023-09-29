package mqttsn.neds.client;

module MqttSNPublisher extends MqttSNClient
{
    parameters:
        string willTopic = default(""); // will topic name
        string willMsg = default(""); // will message
      	bool willFlag = default(false);
        
        @display("i=block/routing;bgb=38,25");
        @class(MqttSNPublisher);
}
