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

package mqttsn.neds.server;

import mqttsn.neds.MqttSNApp;

module MqttSNServer extends MqttSNApp
{
    parameters:
        @class(MqttSNServer);
        
        // time intervals for each state, -1s means forever
        double offlineStateInterval @unit(s) = default(2s);
        double onlineStateInterval @unit(s) = default(20s);
        
        int advertiseInterval @unit(s) = default(900s); // range between 0..65535 seconds (TADV)
        
        double activeClientsCheckInterval @unit(s) = default(500ms); // check interval for verifying active clients
        double asleepClientsCheckInterval @unit(s) = default(500ms); // check interval for verifying asleep clients
        
        int maximumClients = default(10); // maximum clients before congestion
        
        double pendingRetainCheckInterval @unit(s) = default(500ms); // check interval for verifying pending retain messages
        double requestsCheckInterval @unit(s) = default(500ms); // check interval for verifying requests to subscribers
        double registrationsCheckInterval @unit(s) = default(500ms); // check interval for verifying topic registrations
        double awakenSubscriberCheckInterval @unit(s) = default(500ms); // check interval for verifying awaken subscriber
        
        double messagesClearInterval @unit(s) = default(60s); // interval for clearing request messages
}
