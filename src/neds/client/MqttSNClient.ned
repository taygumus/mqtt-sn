package mqttsn.neds.client;

import mqttsn.neds.MqttSNApp;

module MqttSNClient extends MqttSNApp
{
    parameters:
        @class(MqttSNClient);
		@display("i=block/routing;bgb=20,20");
        
        // time intervals for each state, -1s means forever
        double disconnectedStateInterval @unit(s) = default(5s);
        double activeStateInterval @unit(s) = default(120s);
        double lostStateInterval @unit(s) = default(5s);
        double asleepStateInterval @unit(s) = default(5s); // linked to sleep duration field; range between 0..65535 seconds
        
        double checkGatewaysInterval @unit(s) = default(500ms); // check interval for active gateways in the map
        int nadv = default(2); // the number of consecutive times without receiving ADVERTISE (NADV)
        
        double searchGatewayMaxDelay @unit(s) = default(5s); // random number upper bound (TSEARCHGW)
        double maxSearchGatewayInterval @unit(s) = default(20s); // exponential rtx interval will reach a maximum
        
        int temporaryDuration @unit(s) = default(5s); // range between 0..65535 seconds
        
        double gatewayInfoMaxDelay @unit(s) = default(5s); // random number upper bound (TGWINFO)
        
        double checkConnectionInterval @unit(s) = default(500ms); // check interval for client's connection to a gateway/server
        
        int keepAlive @unit(s) = default(60s); // up to a maximum of 65535 seconds, approximately 18 hours
        
        bool cleanSession = default(false);
        
        string itemsJson; // json string containing topic-related items
        double waitingInterval @unit(s) = default(300s); // waiting time before restarting a procedure (TWAIT)
}
