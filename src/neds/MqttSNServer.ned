package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNServer like IApp
{
    parameters:
        string localAddress = default("");
        
        int localPort = default(-1);
        int destPort = default(-1);
        
        double startTime @unit(s) = default(this.advertiseInterval);
        double stopTime @unit(s) = default(-1s);
        
        volatile double advertiseInterval @unit(s);
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);

   	gates:
		input socketIn;
        output socketOut;
}
