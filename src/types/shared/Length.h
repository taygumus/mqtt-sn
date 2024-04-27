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

#ifndef TYPES_LENGTH_H_
#define TYPES_LENGTH_H_

enum Length : uint16_t {
    ZERO_OCTETS = 0,
    ONE_OCTET = 1,
    TWO_OCTETS = 2,
    THREE_OCTETS = 3,
    FOUR_OCTETS = 4,
    CLIENT_ID_OCTETS = 23
};

#endif /* TYPES_LENGTH_H_ */
