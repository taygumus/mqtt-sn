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

#ifndef TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_
#define TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_

struct ItemInfo {
    std::string topicName = "";
    uint16_t topicId = 0; // used only as predefined topic ID
    TopicIdType topicIdType = TopicIdType::NORMAL_TOPIC_ID;
    QoS qos = QoS::QOS_ZERO;
    int subscribeCounter = 0;
    int unsubscribeCounter = 0;
};

#endif /* TYPES_CLIENT_SUBSCRIBER_ITEMINFO_H_ */
