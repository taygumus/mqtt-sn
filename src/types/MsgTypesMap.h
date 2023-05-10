#ifndef TYPES_MSGTYPESMAP_H_
#define TYPES_MSGTYPESMAP_H_

std::map<std::string, std::vector<MsgType>> msgTypesMap = {
    {"MqttSNBase", {MsgType::WILLTOPICREQ, MsgType::WILLMSGREQ, MsgType::PINGRESP}},
    {"MqttSNBaseWithReturnCode", {MsgType::CONNACK, MsgType::WILLTOPICRESP, MsgType::WILLMSGRESP}},
    {"MqttSNBaseWithWillMsg", {MsgType::WILLMSG, MsgType::WILLMSGUPD}},
    {"MqttSNBaseWithWillTopic", {MsgType::WILLTOPIC, MsgType::WILLTOPICUPD}},
    {"MqttSNBaseWithMsgId", {MsgType::PUBREC, MsgType::PUBREL, MsgType::PUBCOMP, MsgType::UNSUBACK}},
    {"MqttSNMsgIdWithTopicIdExtended", {MsgType::REGACK, MsgType::PUBACK}},

    {"MqttSNSearchGw", {MsgType::SEARCHGW}},
    {"MqttSNPingReq", {MsgType::PINGREQ}},
    {"MqttSNDisconnect", {MsgType::DISCONNECT}},
    {"MqttSNAdvertise", {MsgType::ADVERTISE}},
    {"MqttSNGwInfo", {MsgType::GWINFO}},
    {"MqttSNConnect", {MsgType::CONNECT}},
    {"MqttSNRegister", {MsgType::REGISTER}},
    {"MqttSNSubAck", {MsgType::SUBACK}},
    {"MqttSNPublish", {MsgType::PUBLISH}},
    {"MqttSNUnsubscribe", {MsgType::UNSUBSCRIBE}},
    {"MqttSNSubscribe", {MsgType::SUBSCRIBE}}
};

#endif /* TYPES_MSGTYPESMAP_H_ */
