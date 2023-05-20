#ifndef TYPES_MSGTYPESMAP_H_
#define TYPES_MSGTYPESMAP_H_

std::map<std::string, std::vector<MsgType>> msgTypesMap = {
    {"MqttSNBase", {MsgType::WILLTOPICREQ, MsgType::WILLMSGREQ, MsgType::PINGRESP}},
    {"MqttSNBaseWithMsgId", {MsgType::PUBREC, MsgType::PUBREL, MsgType::PUBCOMP, MsgType::UNSUBACK}},
    {"MqttSNBaseWithReturnCode", {MsgType::CONNACK, MsgType::WILLTOPICRESP, MsgType::WILLMSGRESP}},
    {"MqttSNBaseWithWillMsg", {MsgType::WILLMSG, MsgType::WILLMSGUPD}},
    {"MqttSNBaseWithWillTopic", {MsgType::WILLTOPIC, MsgType::WILLTOPICUPD}},
    {"MqttSNMsgIdWithTopicIdPlus", {MsgType::REGACK, MsgType::PUBACK}},

    {"MqttSNAdvertise", {MsgType::ADVERTISE}},
    {"MqttSNConnect", {MsgType::CONNECT}},
    {"MqttSNDisconnect", {MsgType::DISCONNECT}},
    {"MqttSNGwInfo", {MsgType::GWINFO}},
    {"MqttSNPingReq", {MsgType::PINGREQ}},
    {"MqttSNPublish", {MsgType::PUBLISH}},
    {"MqttSNRegister", {MsgType::REGISTER}},
    {"MqttSNSearchGw", {MsgType::SEARCHGW}},
    {"MqttSNSubAck", {MsgType::SUBACK}},
    {"MqttSNSubscribe", {MsgType::SUBSCRIBE}},
    {"MqttSNUnsubscribe", {MsgType::UNSUBSCRIBE}}
};

#endif /* TYPES_MSGTYPESMAP_H_ */
