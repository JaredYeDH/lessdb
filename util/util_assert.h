//
// Created by neverchanje on 15/7/8.
//

#ifndef UTIL_ASSERT_H
#define UTIL_ASSERT_H

#include <stdio.h>
#include <stdlib.h> //abort

/* A no-op that tricks the compiler into thinking a condition is used while
 * definitely not making any code for it.  Used to compile out asserts while
 * avoiding "unused variable" warnings.  The "!" forces the compiler to
 * do the sizeof() on an int, in case "condition" is a bitfield value.
 */
#define UTIL_NIL_CONDITION_(condition) do { \
	(void)sizeof(!(condition));  \
} while(0)

#define UTIL_LIKELY(p)   __builtin_expect(!!(p), 1)
#define UTIL_UNLIKELY(p) __builtin_expect(!!(p), 0)

#ifndef NDEBUG //release mode
#define UTIL_ASSERT(cond) UTIL_NIL_CONDITION_(cond)
#else //debug mode
#define UTIL_ASSERT(cond) \
    do { \
        if(UTIL_UNLIKELY(!cond)) { \
            fprintf(stderr, "%s:%d: Assertion %s failed in %s", \
            __FILE__,__LINE__,#cond,__func__); \
            abort(); \
        } \
    } while(0)
#endif

#endif //UTIL_ASSERT_H