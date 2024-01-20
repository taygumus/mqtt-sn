package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNApp like IApp
{
    parameters:
        @display("i=block/app");
        @class(MqttSNApp);
        @lifecycleSupport;

        string localAddress = default("");
        string broadcastAddress = default("255.255.255.255");

        int localPort = default(-1);
        int destPort = default(-1);

        double retransmissionInterval @unit(s) = default(10s); // retransmission retry interval (TRETRY)
        int retransmissionCounter = default(3); // retransmission retry counter (NRETRY)
        
        string predefinedTopicsJson; // json string with topic names and their associated predefined ids

    gates:
        input socketIn;
        output socketOut;
}
