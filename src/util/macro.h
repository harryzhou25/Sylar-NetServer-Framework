#ifndef _SYLAR_MACRO_H
#define _SYLAR_MACRO_H

#include "util/util.h"
#include "log/logger.h"
#include <assert.h>

#define Log_Assert(x) \
    if(!(x)) {\
        Log_Error(Root_Logger()) << "Assertion: " << x << sylar::BacktraceToString();\
        assert(x); \
    }

#endif //_SYLAR_MACRO_H