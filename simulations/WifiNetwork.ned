package mqttsn.simulations;

import inet.node.wireless.AccessPoint;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211RadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.WirelessHost;

@license(LGPL);

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

