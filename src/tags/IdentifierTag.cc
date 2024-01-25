#include "IdentifierTag.h"

namespace mqttsn {

void IdentifierTag::setIdentifier(unsigned identifier)
{
    id = identifier;
}

unsigned IdentifierTag::getIdentifier() const
{
    return id;
}

} /* namespace mqttsn */
