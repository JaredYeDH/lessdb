//
// Created by neverchanje on 8/25/15.
//

#ifndef LESSDB_SDS_H
#define LESSDB_SDS_H

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

/**
 * A Simple Dynamic String, which is designed for better
 * memory-efficiency compared to std::string.
 *
 * Memory Layout of Sds:
 * <STRING><FREE><NULL>
 *
 * Sds implicitly adds a NULL term('\0') at the end of the
 * actual content of the string.
 * The NULL term is not counted in capacity().
 */
class Sds {

public:

    Sds() : len_(0), free_(0), str_(NULL) { }

    Sds(char *s, uint32_t newlen);

    explicit Sds(uint32_t newlen) : Sds(NULL, newlen) { }

    //Allows implicit inversion
    Sds(char *s) : Sds(s, strlen(s)) { }

    //copyable
    Sds(Sds &s) : Sds(s.data(), s.length()) { }
    Sds &operator=(const Sds &s) {
        len_ = s.length();
        free_ = s.capacity() - s.length();
        str_ = (char *)realloc(str_, s.capacity());
        memcpy(str_, s.data(), s.length()+1);
        return *this;
    }

    ~Sds() {
        free(str_);
    }

    uint32_t length() const {
        return len_;
    }

    uint32_t capacity() const {
        return len_ + free_;
    }

    void append(const Sds &s);

    void clear(void) {
        *str_ = '\0';
        free_ += len_;
        len_ = 0;
    }

    char *data() const {
        return str_;
    }

    char operator[](size_t n) const {
        return str_[n];
    }

private:

    uint32_t len_;

    uint32_t free_;

    char *str_;
};

#endif //LESSDB_SDS_H
