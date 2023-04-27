#ifndef TYPES_FLAG_H_
#define TYPES_FLAG_H_

enum Flag : uint16_t {
    TOPIC_ID_TYPE = 0,
    CLEAN_SESSION = 2,
    WILL = 3,
    RETAIN = 4,
    QUALITY_OF_SERVICE = 5,
    DUP = 7
};

#endif /* TYPES_FLAG_H_ */
