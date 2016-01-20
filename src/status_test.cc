//
// Created by neverchanje on 8/24/15.
//

#include "Status.h"

#include <iostream>

class Task {
public:
    Status perform() { return Status::OK(); };
    Status perform2() { return Status::Failed(); }
};

int main() {
    Task task;

    Status stat = task.perform();
    if(stat.IsOK() ) {
        std::cout<< stat.ToString()<<std::endl;
    }

    stat = task.perform2();
    if(stat.IsFailed() ) {
        std::cout<< stat.ToString()<<std::endl;
    }
}
