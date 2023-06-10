package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNClient like IApp
{
    parameters:
        string localAddress = default("");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        volatile double checkGatewaysInterval @unit(s) = default(1s); // active gateways check in the map
        int nadv = default(2); // the number of consecutive times without receiving ADVERTISE
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNClient);

   	gates:
		input socketIn;
        output socketOut;
}
