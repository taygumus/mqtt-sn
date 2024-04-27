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

#ifndef TYPES_SERVER_PUBLISHERINFO_H_
#define TYPES_SERVER_PUBLISHERINFO_H_

struct PublisherInfo {
    bool will = false;
    QoS willQoS = QoS::QOS_ZERO;
    bool willRetain = false;
    std::string willTopic = "";
    std::string willMsg = "";
    std::map<uint16_t, DataInfo> messages;
};

#endif /* TYPES_SERVER_PUBLISHERINFO_H_ */
