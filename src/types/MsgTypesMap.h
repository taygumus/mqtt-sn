#ifndef TYPES_MSGTYPESMAP_H_
#define TYPES_MSGTYPESMAP_H_

std::map<std::string, std::vector<MsgType>> msgTypesMap = {
    {"MqttSNBase", {MsgType::WILLTOPICREQ, MsgType::WILLMSGREQ, MsgType::PINGRESP}},
    {"MqttSNSearchGw", {MsgType::SEARCHGW}},
    {"MqttSNBaseWithReturnCode", {MsgType::CONNACK, MsgType::WILLTOPICRESP, MsgType::WILLMSGRESP}},
    {"MqttSNBaseWithWillMsg", {MsgType::WILLMSG, MsgType::WILLMSGUPD}},
    {"MqttSNBaseWithMsgId", {MsgType::PUBREC, MsgType::PUBREL, MsgType::PUBCOMP, MsgType::UNSUBACK}},
    {"MqttSNPingReq", {MsgType::PINGREQ}},
    {"MqttSNDisconnect", {MsgType::DISCONNECT}},
};

#endif /* TYPES_MSGTYPESMAP_H_ */
