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
        client1: WirelessHost {
            @display("p=224,111");
        }
        client2: WirelessHost {
            @display("p=224,189");
        }
        client3: WirelessHost {
            @display("p=224,270");
        }
        server: WirelessHost {
            @display("p=468,111");
        }
        
}

