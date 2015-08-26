//
// Created by neverchanje on 8/25/15.
//

#include "sds.h"
#include "test_cond.h"
#include <iostream>
using namespace std;

int main() {

    {
        Sds x("foo");
        TEST_COND("Create a string and obtain the length",
                  x.length()==3 && memcmp(x.data(), "foo\0", 4) == 0);
    }

    {
        Sds x("foo", 2);
        TEST_COND("Create a string with specified length",
                  x.length()==2 && memcmp(x.data(), "fo\0", 3) == 0);

        x.append("bar");
        TEST_COND("Strings concatenation",
                  x.length() == 5 && memcmp(x.data(),"fobar\0",6) == 0);

        x = Sds("a");
        TEST_COND("operator=() against an originally longer string",
                  x.length() == 1 && memcmp(x.data(),"a\0",2) == 0);

        x = "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk";
        TEST_COND("operator= against an originally shorter string",
                  x.length() == 33 &&
                  memcmp(x.data(),"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0);

        TEST_COND("Get the 4th byte of the string", x[2] == 'x');

        x.clear();
        TEST_COND("Clear the string",
                  x.length() == 0 && memcmp(x.data(), "\0", 1) == 0);
    }
}