package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNClient like IApp
{
    parameters:
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNClient);

   	gates:
		input socketIn;
        output socketOut;
}
