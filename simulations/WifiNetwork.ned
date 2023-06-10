package mqttsn.simulations;

import inet.node.wireless.AccessPoint;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.WirelessHost;

@license(LGPL);

network WifiNetwork
{
    @display("bgb=561,334");

    submodules:
        accessPoint: AccessPoint {
            @display("p=370,42");
        }
        radioMedium: Ieee80211RadioMedium {
            @display("p=60,42");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=60,111");
        }
        client1: WirelessHost {
            @display("p=201,132");
        }
        client2: WirelessHost {
            @display("p=201,265");
        }
        server1: WirelessHost {
            @display("p=494,265");
        }
        server2: WirelessHost {
            @display("p=494,132");
        }
}

