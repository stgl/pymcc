#ifndef MCC_ENUMS_H
#define MCC_ENUMS_H

#include <boost/cstdint.hpp>
// Interface to MCC library for Python (or C++) bindings

typedef enum
{
    NONE,
    EQUAL_INTERVAL,
    RANDOM
} subSamplingType;

#endif // MCC_H
