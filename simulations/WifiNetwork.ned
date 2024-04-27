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

package mqttsn.simulations;

import inet.node.wireless.AccessPoint;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.WirelessHost;

network WifiNetwork
{
    @display("bgb=713,388");

    submodules:
        accessPoint: AccessPoint {
            @display("p=415,35");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=60,42");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=60,110");
        }
        publisher1: WirelessHost {
            @display("p=207,140");
        }
        publisher2: WirelessHost {
            @display("p=207,252");
        }
        server: WirelessHost {
            @display("p=415,190");
        }
        subscriber1: WirelessHost {
            @display("p=633,90");
        }
        subscriber2: WirelessHost {
            @display("p=633,167");
        }
        subscriber3: WirelessHost {
            @display("p=633,243");
        }
        subscriber4: WirelessHost {
            @display("p=633,325");
        }
}

