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

#ifndef TYPES_RETURNCODE_H_
#define TYPES_RETURNCODE_H_

enum ReturnCode : uint8_t {
    ACCEPTED = 0x00,
    REJECTED_CONGESTION = 0x01,
    REJECTED_INVALID_TOPIC_ID = 0x02,
    REJECTED_NOT_SUPPORTED = 0x03
};

#endif /* TYPES_RETURNCODE_H_ */
