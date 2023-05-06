#include "MqttSNBase.h"
#include "types/MsgTypesMap.h"
#include "types/Length.h"
#include "cxxabi.h"

namespace mqttsn {

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

void MqttSNBase::addLength(uint16_t octets, uint16_t prevOctets)
{
    if (octets == prevOctets)
        return;

    uint16_t current = getLength();

    if (current < prevOctets)
        throw omnetpp::cRuntimeError("Previous octets cannot exceed current message length");

    setLength(current - prevOctets + octets);
}

std::string MqttSNBase::getClassName(std::string mangledName)
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

MqttSNBase::MqttSNBase()
{
    addLength(Length::TWO_OCTETS);
}

uint16_t MqttSNBase::getLength()
{
    if (length.size() == 1) {
        return static_cast<uint16_t>(length[0]);
    }

    if (length.size() == 3 && length[0] == 0x01) {
        return static_cast<uint16_t>(length[2]) << 8 | static_cast<uint16_t>(length[1]);
    }

    return 0;
}

uint16_t MqttSNBase::getAvailableLength()
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
        throw omnetpp::cRuntimeError("Unknown class name");
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
