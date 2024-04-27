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

module MqttSNSubscriber extends MqttSNClient
{
    parameters:
        @class(MqttSNSubscriber);
        
        double subscriptionInterval @unit(s) = default(5s); // subscription interval for new topics
        int subscriptionLimit = default(-1); // maximum subscriptions, -1 for unlimited
        
        double unsubscriptionInterval @unit(s) = default(20s); // unsubscription interval from topics
        int unsubscriptionLimit = default(-1); // maximum unsubscriptions, -1 for unlimited
}
