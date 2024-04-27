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

#ifndef TAGS_IDENTIFIERTAG_H_
#define TAGS_IDENTIFIERTAG_H_

#include "inet/common/TagBase_m.h"

namespace mqttsn {

class IdentifierTag : public inet::TagBase
{
    private:
        unsigned id = 0;

    public:
        IdentifierTag() {};

        void setIdentifier(unsigned identifier);
        unsigned getIdentifier() const;

        ~IdentifierTag() {};
};

} /* namespace mqttsn */

#endif /* TAGS_IDENTIFIERTAG_H_ */
