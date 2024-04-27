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

#ifndef TYPES_SERVER_REQUESTINFO_H_
#define TYPES_SERVER_REQUESTINFO_H_

struct RequestInfo {
    inet::clocktime_t requestTime = 0;
    int retransmissionCounter = 0;
    inet::L3Address subscriberAddress;
    int subscriberPort = 0;
    MsgType messageType = MsgType::PUBLISH;
    bool sendAtLeastOnce = true;
    uint16_t messagesKey = 0;
    uint16_t retainMessagesKey = 0;
};

#endif /* TYPES_SERVER_REQUESTINFO_H_ */
