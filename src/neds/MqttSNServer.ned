package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNServer like IApp
{
    parameters:
        string localAddress = default("");
        string broadcastAddress = default("255.255.255.255");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        double startAdvertise @unit(s) = default(this.advertiseInterval);
        double stopAdvertise @unit(s) = default(-1s);
        int advertiseInterval @unit(s); // range between 0..65535 seconds (TADV)
        
        int maxConnectedClients = default(10); // maximum connected clients before congestion
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);

   	gates:
		input socketIn;
        output socketOut;
}
