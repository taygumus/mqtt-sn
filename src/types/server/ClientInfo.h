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

#ifndef TYPES_SERVER_CLIENTINFO_H_
#define TYPES_SERVER_CLIENTINFO_H_

struct ClientInfo {
    std::string clientId = "";
    ClientType clientType = ClientType::CLIENT;
    uint16_t keepAliveDuration = 0;
    uint16_t sleepDuration = 0;
    ClientState currentState = ClientState::DISCONNECTED;
    inet::clocktime_t lastReceivedMsgTime = 0;
    bool sentPingReq = false;
};

#endif /* TYPES_SERVER_CLIENTINFO_H_ */
