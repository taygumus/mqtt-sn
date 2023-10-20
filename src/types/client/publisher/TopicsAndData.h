#ifndef TYPES_CLIENT_PUBLISHER_TOPICSANDDATA_H_
#define TYPES_CLIENT_PUBLISHER_TOPICSANDDATA_H_

struct TopicsAndData {
    std::string topicName;
    int counter = 0;
    std::vector<std::string> data;
};

#endif /* TYPES_CLIENT_PUBLISHER_TOPICSANDDATA_H_ */
