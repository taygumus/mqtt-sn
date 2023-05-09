#include "MqttSNBase.h"
#include "types/MsgTypesMap.h"
#include "types/Length.h"
#include "cxxabi.h"

namespace mqttsn {

/* Private */
void MqttSNBase::setLength(uint16_t octets)
{
    if (length.size() > 0)
        length.clear();

    if (octets <= UINT8_MAX) {
        length.push_back(static_cast<uint8_t>(octets));
    }
    else {
        length.push_back(0x01);
        length.push_back(static_cast<uint8_t>(octets & 0xFF));
        length.push_back(static_cast<uint8_t>((octets >> 8) & 0xFF));
   }
}

/* Protected */
void MqttSNBase::addLength(uint16_t octets, uint16_t prevOctets)
{
    if (octets == prevOctets)
        return;

    uint16_t current = getLength();

    if (current < prevOctets)
        throw omnetpp::cRuntimeError("Previous octets cannot exceed current message length");

    setLength(current - prevOctets + octets);
}

void MqttSNBase::setClientId(std::string id, std::string& clientId)
{
    uint16_t length = id.length();
    uint16_t prevLength;

    if (length >= Length::ONE_OCTET && length <= Length::CLIENT_ID_OCTETS) {
        prevLength = clientId.size();
        clientId = id;
    }
    else {
        throw omnetpp::cRuntimeError("Client ID length must be within the range of 1 to 23 characters");
    }

    addLength(length, prevLength);
}

void MqttSNBase::setOptionalField(uint32_t value, uint16_t octets, uint32_t& field)
{
    if (value == field)
        return;

    uint16_t length = value == 0 ? Length::ZERO_OCTETS : octets;
    uint16_t prevLength = field == 0 ? Length::ZERO_OCTETS : octets;

    field = value;

    if (length == prevLength)
        return;

    addLength(length, prevLength);
}

void MqttSNBase::setStringField(std::string value, std::string error, std::string& field)
{
    uint16_t length = value.length();
    uint16_t prevLength;

    if (length <= getAvailableLength()) {
        prevLength = field.size();
        field = value;
    }
    else {
        throw omnetpp::cRuntimeError("%s", error.c_str());
    }

    addLength(length, prevLength);
}

void MqttSNBase::setFlag(uint8_t value, Flag position, uint8_t& flags)
{
    flags = (flags & ~(0b11 << position)) | (value << position);
}

void MqttSNBase::setBooleanFlag(bool value, Flag position, uint8_t& flags)
{
    flags = (flags & ~(1 << position)) | (value << position);
}

std::string MqttSNBase::getClassName(std::string mangledName) const
{
    int status;
    char* demangledName = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

    if (status == 0) {
        std::string className(demangledName);
        free(demangledName);

        return className.substr(className.find("::") + 2);
    }

    throw omnetpp::cRuntimeError("Error getting class name");
}

uint8_t MqttSNBase::getFlag(Flag position, uint8_t flags) const
{
    return (flags >> position) & 0b11;
}

bool MqttSNBase::getBooleanFlag(Flag position, uint8_t flags) const
{
    return (flags & (1 << position)) != 0;
}

/* Public */
MqttSNBase::MqttSNBase()
{
    addLength(Length::TWO_OCTETS);
}

uint16_t MqttSNBase::getLength() const
{
    if (length.size() == 1) {
        return static_cast<uint16_t>(length[0]);
    }

    if (length.size() == 3 && length[0] == 0x01) {
        return static_cast<uint16_t>(length[2]) << 8 | static_cast<uint16_t>(length[1]);
    }

    return 0;
}

uint16_t MqttSNBase::getAvailableLength() const
{
    return UINT16_MAX - getLength();
}

void MqttSNBase::setMsgType(MsgType messageType)
{
    std::string className = getClassName(typeid(*this).name());
    std::vector<MsgType> types;

    try {
        types = msgTypesMap.at(className);
    }
    catch (std::out_of_range e) {
        throw omnetpp::cRuntimeError("Class without message type");
    }

    if (std::find(types.begin(), types.end(), messageType) == types.end()) {
        throw omnetpp::cRuntimeError("Incorrect message type");
    }

    msgType = messageType;
}

MsgType MqttSNBase::getMsgType() const
{
    return msgType;
}

} /* namespace mqttsn */
