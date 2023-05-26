package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNClient like IApp
{
    parameters:
        string localAddress = default("");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNClient);

   	gates:
		input socketIn;
        output socketOut;
}
