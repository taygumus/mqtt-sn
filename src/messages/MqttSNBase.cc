#include "MqttSNBase.h"
#include "types/MsgTypesMap.h"
#include "cxxabi.h"

namespace mqttsn {

std::string MqttSNBase::getClassName(std::string mangledName)
{
    int status;
    char* demangledName = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

    if (status == 0) {
        std::string className(demangledName);
        free(demangledName);

        return className.substr(className.find("::") + 2);
    }

    throw omnetpp::cRuntimeError("Error with getting the class name");
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
