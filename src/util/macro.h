#ifndef _SYLAR_MACRO_H
#define _SYLAR_MACRO_H

#include "util/util.h"
#include "log/logger.h"
#include <assert.h>

#define Assert(x) \
    if(!(x)) {\
        Log_Error(Root_Logger()) << "Assertion: " << x << sylar::BacktraceToString();\
        assert(x); \
    }

#define Assert_Commit(x, s) \
    if(!(x)) {\
        Log_Error(Root_Logger()) << "Assertion: " << x << sylar::BacktraceToString(64, 2, s);\
        assert(x); \
    }

#endif //_SYLAR_MACRO_H