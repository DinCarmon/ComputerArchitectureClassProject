#ifndef FLIP_FLOP_H
#define FLIP_FLOP_H

#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

#define CREATE_FLIP_FLOP_OF_TYPE(TYPE)                      \
    typedef struct flipFlop_##TYPE{                         \
        TYPE now;                                           \
        TYPE updated;                                       \
    } FlipFlop_##TYPE;                      

CREATE_FLIP_FLOP_OF_TYPE(uint32_t)
CREATE_FLIP_FLOP_OF_TYPE(CacheLineStatus)
CREATE_FLIP_FLOP_OF_TYPE(CacheLine)
CREATE_FLIP_FLOP_OF_TYPE(BusShareStatus)
CREATE_FLIP_FLOP_OF_TYPE(BusCmd)
CREATE_FLIP_FLOP_OF_TYPE(int32_t)
CREATE_FLIP_FLOP_OF_TYPE(bool)
CREATE_FLIP_FLOP_OF_TYPE(BusStatus)

#define UPDATE_FLIP_FLOP(obj, defaultKeepPreviousValue)     \
   {                                                        \
        obj.now = obj.updated;                              \
        if(!defaultKeepPreviousValue)                       \
            obj.updated = 0;                                \
                                                            \
   }
    

#endif