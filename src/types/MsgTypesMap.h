#ifndef TYPES_MSGTYPESMAP_H_
#define TYPES_MSGTYPESMAP_H_

std::map<std::string, std::vector<MsgType>> msgTypesMap = {
    {"MqttSNBase", {MsgType::WILLTOPICREQ, MsgType::WILLMSGREQ, MsgType::PINGRESP}},
    {"MqttSNSearchGw", {MsgType::SEARCHGW}},
    {"MqttSNPingReq", {MsgType::PINGREQ}},
    {"MqttSNDisconnect", {MsgType::DISCONNECT}},
    {"MqttSNAdvertise", {MsgType::ADVERTISE}},
    {"MqttSNGwInfo", {MsgType::GWINFO}},
    {"MqttSNConnect", {MsgType::CONNECT}},

    {"MqttSNBaseWithReturnCode", {MsgType::CONNACK, MsgType::WILLTOPICRESP, MsgType::WILLMSGRESP}},
    {"MqttSNBaseWithWillMsg", {MsgType::WILLMSG, MsgType::WILLMSGUPD}},
    {"MqttSNBaseWithMsgId", {MsgType::PUBREC, MsgType::PUBREL, MsgType::PUBCOMP, MsgType::UNSUBACK}},
    {"MqttSNBaseWithWillTopic", {MsgType::WILLTOPIC, MsgType::WILLTOPICUPD}},
};

#endif /* TYPES_MSGTYPESMAP_H_ */
