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

import mqttsn.neds.MqttSNApp;

module MqttSNClient extends MqttSNApp
{
    parameters:
        @class(MqttSNClient);
        
        // time intervals for each state, -1s means forever
        double disconnectedStateInterval @unit(s) = default(5s);
        double activeStateInterval @unit(s) = default(30s);
        double lostStateInterval @unit(s) = default(5s);
        double asleepStateInterval @unit(s) = default(60s); // linked to sleep duration field; range between 0..65535 seconds
        
        double checkGatewaysInterval @unit(s) = default(500ms); // check interval for active gateways in the map
        int nadv = default(2); // the number of consecutive times without receiving ADVERTISE (NADV)
        
        double searchGatewayMaxDelay @unit(s) = default(5s); // random number upper bound (TSEARCHGW)
        double maxSearchGatewayInterval @unit(s) = default(20s); // exponential rtx interval will reach a maximum
        
        int temporaryDuration @unit(s) = default(5s); // range between 0..65535 seconds
        
        double gatewayInfoMaxDelay @unit(s) = default(5s); // random number upper bound (TGWINFO)
        
        double checkConnectionInterval @unit(s) = default(500ms); // check interval for client's connection to a gateway/server
        
        int keepAlive @unit(s) = default(60s); // up to a maximum of 65535 seconds, approximately 18 hours
        
        bool cleanSession = default(false); // controls session cleanup: deletes will data for publishers, subscriptions for subscribers
        
        string itemsJson; // json string containing topic-related items
        double waitingInterval @unit(s) = default(30s); // waiting time before restarting a procedure (TWAIT)
}
