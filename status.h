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
 *     if( stat.failed() ) {
 *          ...
 *     }
 * }
 * </code>
 */

class Status {

    //copyable

public:

    Status() : status(NULL) {};

    static Status OK() { return Status("OK"); };

    static Status Failed() { return Status("Failed"); };

    std::string toString() { return status; }

    bool ok() const { return status == "OK"; }

    bool failed() const { return status == "Failed"; }

private:

    explicit Status(const std::string& msg) {
        status = msg;
    }

    std::string status;

};

#endif //LESSDB_STATUS_H
