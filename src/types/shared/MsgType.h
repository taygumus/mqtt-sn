//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef TYPES_MSGTYPE_H_
#define TYPES_MSGTYPE_H_

enum MsgType : uint8_t {
    ADVERTISE = 0x00,
    SEARCHGW = 0x01,
    GWINFO = 0x02,
    CONNECT = 0x04,
    CONNACK = 0x05,
    WILLTOPICREQ = 0x06,
    WILLTOPIC = 0x07,
    WILLMSGREQ = 0x08,
    WILLMSG = 0x09,
    REGISTER = 0x0A,
    REGACK = 0x0B,
    PUBLISH = 0x0C,
    PUBACK = 0x0D,
    PUBCOMP = 0x0E,
    PUBREC = 0x0F,
    PUBREL = 0x10,
    SUBSCRIBE = 0x12,
    SUBACK = 0x13,
    UNSUBSCRIBE = 0x14,
    UNSUBACK = 0x15,
    PINGREQ = 0x16,
    PINGRESP = 0x17,
    DISCONNECT = 0x18,
    WILLTOPICUPD = 0x1A,
    WILLTOPICRESP = 0x1B,
    WILLMSGUPD = 0x1C,
    WILLMSGRESP = 0x1D
};

#endif /* TYPES_MSGTYPE_H_ */
