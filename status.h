//
// Created by neverchanje on 8/24/15.
//

#ifndef LESSDB_STATUS_H
#define LESSDB_STATUS_H

#include <string>

/**
 * `Status` is an utility class, which is only allowed for internal usage.
 *
 * Example:
 * <code>
 * class Task {
 * public:
 *     Status perform();
 * };
 *
 * int main() {
 *     Task task;
 *     Status stat = task.perform();
 *     if( stat.isFailed() ) {
 *          ...
 *     }
 * }
 * </code>
 */

class Status {

    //copyable

public:

    static Status OK() { return Status(Code::kOK); };

    static Status Failed() { return Status(Code::kFailed); };

    std::string toString() {
        switch (status_) {
            case Code::kOK:
                return "OK";
            case Code::kFailed:
                return "Failed";
            default:
                return "Unknown";
        }
    }

    bool isOK() const { return status_ == Code::kOK; }

    bool isFailed() const { return status_ == Code::kFailed; }

private:

    Status() :status_(Code::kOK) {};

    enum Code {
        kOK = 0,
        kFailed = 1
    };

    explicit Status(Code status) {
        status_ = status;
    }

    Code status_;
};

#endif //LESSDB_STATUS_H
