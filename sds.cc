//
// Created by neverchanje on 8/25/15.
//

#include "sds.h"

Sds::Sds(char *s, uint32_t newlen) : len_(newlen), free_(0) {
    if (s == NULL) {
        str_ = (char *) calloc(1, newlen + 1);
        assert(str_ != NULL);
    } else {
        str_ = (char *) malloc(newlen + 1);
        assert(str_ != NULL);
        memcpy(str_, s, newlen);
        str_[newlen] = '\0';
    }
}

void Sds::append(const Sds &s) {
    size_t len = s.length();
    char *data = s.data();

    if (free_ >= len) {
        memcpy(str_ + len_, data, len);
    } else {
        str_ = (char *) realloc(str_, len_ + len);
        assert(str_ != NULL);
        memcpy(str_ + len_, data, len);
    }

    free_ = sizeof(str_) - len - len_;
    len_ += len;
}
