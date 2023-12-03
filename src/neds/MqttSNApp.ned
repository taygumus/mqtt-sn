package mqttsn.neds;

import inet.applications.contract.IApp;

simple MqttSNApp like IApp
{
    parameters:
        @class(MqttSNApp);

        string localAddress = default("");
        string broadcastAddress = default("255.255.255.255");

        int localPort = default(-1);
        int destPort = default(-1);

        double retransmissionInterval @unit(s) = default(10s); // retransmission retry interval (TRETRY)
        int retransmissionCounter = default(3); // retransmission retry counter (NRETRY)

    gates:
        input socketIn;
        output socketOut;
}
