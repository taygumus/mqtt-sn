package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNServer like IApp
{
    parameters:
        double startTime @unit(s) = default(this.advertiseInterval);
        double stopTime @unit(s) = default(-1s);
        volatile double advertiseInterval @unit(s);
        
        string localAddress = default("");
        int localPort = default(-1);
        
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);

   	gates:
		input socketIn;
        output socketOut;
}
