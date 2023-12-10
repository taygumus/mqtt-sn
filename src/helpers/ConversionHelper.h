#ifndef HELPERS_CONVERSIONHELPER_H_
#define HELPERS_CONVERSIONHELPER_H_

#include "BaseHelper.h"
#include "types/shared/QoS.h"
#include "types/shared/TopicIdType.h"

namespace mqttsn {

class ConversionHelper : public BaseHelper
{
    public:
        static QoS intToQoS(int value);
        static int qosToInt(QoS value);
        static TopicIdType stringToTopicIdType(const std::string& idType);
        static std::string topicIdTypeToString(TopicIdType idType);
};

} /* namespace mqttsn */

#endif /* HELPERS_CONVERSIONHELPER_H_ */
