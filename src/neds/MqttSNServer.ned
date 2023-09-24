package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNServer like IApp
{
    parameters:
        string localAddress = default("");
        string broadcastAddress = default("255.255.255.255");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        // time intervals for each state, -1s means forever
        double offlineStateInterval @unit(s) = default(30s);
        double onlineStateInterval @unit(s) = default(30s);
        
        int advertiseInterval @unit(s); // range between 0..65535 seconds (TADV)
        
        double activeClientsCheckInterval @unit(s) = default(500ms); // check interval for verifying active clients
        double asleepClientsCheckInterval @unit(s) = default(500ms); // check interval for verifying asleep clients
        
        int maximumClients = default(10); // maximum clients before congestion
        double clientsClearInterval @unit(s) = default(120s); // clear interval for lost and disconnected clients
        double maximumInactivityTime @unit(s) = default(60s);
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);

   	gates:
		input socketIn;
        output socketOut;
}
