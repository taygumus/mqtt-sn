package mqttsn.modules;

import inet.applications.udpapp.UdpBasicApp;

simple MqttSNServer extends UdpBasicApp
{
    parameters:
        @display("i=block/routing;bgb=100,82");
        @class(MqttSNServer);
}
