//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

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
        
        double packetBER = default(0); // packet bit error rate
        
        string predefinedTopicsJson; // json string with topic names and their associated predefined ids

    gates:
        input socketIn;
        output socketOut;
}
