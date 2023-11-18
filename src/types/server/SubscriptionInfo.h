#ifndef TYPES_SERVER_SUBSCRIPTIONINFO_H_
#define TYPES_SERVER_SUBSCRIPTIONINFO_H_

struct SubscriptionInfo {
    std::map<std::pair<inet::L3Address, int>, SubscriberInfo> subscribers;
};

#endif /* TYPES_SERVER_SUBSCRIPTIONINFO_H_ */
