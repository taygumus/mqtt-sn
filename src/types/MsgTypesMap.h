#ifndef TYPES_MSGTYPESMAP_H_
#define TYPES_MSGTYPESMAP_H_

std::map<std::string, std::vector<MsgType>> msgTypesMap = {
    {"MqttSNBase", {MsgType::WILLTOPICREQ, MsgType::WILLMSGREQ, MsgType::PINGRESP}},
    {"MqttSNSearchGw", {MsgType::SEARCHGW}},
};

#endif /* TYPES_MSGTYPESMAP_H_ */
