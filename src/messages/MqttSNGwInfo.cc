#include "MqttSNGwInfo.h"
#include "types/Length.h"

namespace mqttsn {

MqttSNGwInfo::MqttSNGwInfo()
{
    MqttSNBase::addLength(Length::ONE_OCTET);
}

void MqttSNGwInfo::setGwId(uint8_t gatewayId)
{
    gwId = gatewayId;
}

uint8_t MqttSNGwInfo::getGwId() const
{
    return gwId;
}

void MqttSNGwInfo::setGwAdd(std::string gatewayAddress)
{
    // parse the textual IP address
    std::vector<std::string> ipParts;
    std::string ipPart;
    std::istringstream ipStream(gatewayAddress);

    while (std::getline(ipStream, ipPart, '.')) {
        ipParts.push_back(ipPart);
    }

    // assign the IP address bytes to the gwAdd field
    uint32_t result = 0;

    if (ipParts.size() == 4) {
        result = (std::stoul(ipParts[0]) << 24) |
                 (std::stoul(ipParts[1]) << 16) |
                 (std::stoul(ipParts[2]) << 8) |
                 (std::stoul(ipParts[3]));
    }

    MqttSNBase::setOptionalField(
            result,
            Length::FOUR_OCTETS,
            gwAdd
    );
}

std::string MqttSNGwInfo::getGwAdd() const
{
    // if gwAdd is 0, return an empty string
    if (gwAdd == 0) {
        return "";
    }

    // convert the gwAdd field back to a textual IP address
    std::ostringstream ipStream;
    ipStream << ((gwAdd >> 24) & 0xFF) << '.'
             << ((gwAdd >> 16) & 0xFF) << '.'
             << ((gwAdd >> 8) & 0xFF) << '.'
             << (gwAdd & 0xFF);

    return ipStream.str();
}

void MqttSNGwInfo::setGwPort(uint16_t gatewayPort)
{
    uint32_t field = gwPort;
    MqttSNBase::setOptionalField(gatewayPort, Length::TWO_OCTETS, field);
    gwPort = field;
}

uint16_t MqttSNGwInfo::getGwPort() const
{
    return gwPort;
}

} /* namespace mqttsn */
