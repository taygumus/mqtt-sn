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
