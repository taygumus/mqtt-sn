package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNClient like IApp
{
    parameters:
        string localAddress = default("");
        string broadcastAddress = default("255.255.255.255");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        // time intervals for each state, -1s means forever
        double disconnectedStateInterval @unit(s) = default(2s);
        double activeStateInterval @unit(s) = default(2s);
        double lostStateInterval @unit(s) = default(2s);
        double asleepStateInterval @unit(s) = default(2s);
        double awakeStateInterval @unit(s) = default(2s);
        
        double checkGatewaysInterval @unit(s) = default(1s); // active gateways check in the map
        int nadv = default(2); // the number of consecutive times without receiving ADVERTISE (NADV)
        
        double searchGatewayMaxDelay @unit(s) = default(5s); // random number upper bound (TSEARCHGW)
        double maxSearchGatewayInterval @unit(s) = default(100s); // exponential rtx interval will reach a maximum
        
        int temporaryDuration @unit(s) = default(5s); // range between 0..65535 seconds
        
        double gatewayInfoMaxDelay @unit(s) = default(3s); // random number upper bound (TGWINFO)
        
        double checkConnectionInterval @unit(s) = default(500ms); // check interval for client's connection to a gateway/server
        
        string willTopic = default(""); // will topic name
        string willMsg = default(""); // will message
        
        int keepAlive @unit(s) = default(2s); // up to a maximum of 65535 seconds, approximately 18 hours
        
        bool dupFlag = default(false);
        int qosFlag = default(0); // valid qos values: 0, 1, 2 and -1
        bool retainFlag = default(false);
        bool willFlag = default(false);
        bool cleanSessionFlag = default(false);
        int topicIdTypeFlag = default(0); // valid topicIdType values: 0, 1 and 2
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNClient);

   	gates:
		input socketIn;
        output socketOut;
}
