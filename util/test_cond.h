//
// Created by neverchanje on 8/25/15.
//

#ifndef UTIL_TEST_COND_H
#define UTIL_TEST_COND_H

static int __test_num = 0;
static int __failed_tests = 0;

#define TEST_COND(descr,_c) do { \
    __test_num++; printf("%d - %s: ", __test_num, descr); \
    if(_c) printf("PASSED\n"); else {printf("FAILED\n"); __failed_tests++;} \
} while(0)

#endif //UTIL_TEST_COND_H