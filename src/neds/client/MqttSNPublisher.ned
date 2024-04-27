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

package mqttsn.neds.client;

module MqttSNPublisher extends MqttSNClient
{
    parameters:
        @class(MqttSNPublisher);
        
        bool will = default(false);
        int willQoS = default(0); // valid QoS values: 0, 1, 2 and -1
        bool willRetain = default(false);
        string willTopic = default(""); // will topic name
        string willMsg = default(""); // will message
        
        double registrationInterval @unit(s) = default(5s); // registration interval for new topic names
        int registrationLimit = default(-1); // maximum registrations (predefined topics are excluded), -1 for unlimited
        
        double publishInterval @unit(s) = default(10s); // publish interval for new messages
        int publishLimit = default(-1); // maximum publications, -1 for unlimited
        
        double publishMinusOneInterval @unit(s) = default(20s); // publish interval for new messages with QoS -1
        int publishMinusOneLimit = default(-1); // maximum publications with QoS -1, -1 for unlimited
        string publishMinusOneDestAddress = default(""); // address of the gateway for QoS -1 publications
        int publishMinusOneDestPort = default(-1); // port number of the gateway for QoS -1 publications
}
