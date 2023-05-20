package mqttsn.simulations;

import inet.node.wireless.AccessPoint;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.WirelessHost;

@license(LGPL);

network WifiNetwork
{
    @display("bgb=530,334");

    submodules:
        accessPoint: AccessPoint {
            @display("p=357,28");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=60,42");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=60,111");
        }
        client: WirelessHost {
            @display("p=222,172");
        }
        server1: WirelessHost {
            @display("p=467,263");
        }
        server2: WirelessHost {
            @display("p=467,111");
        }

}

