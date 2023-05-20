package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNServer like IApp
{
    parameters:
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);

   	gates:
		input socketIn;
        output socketOut;
}
