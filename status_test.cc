//
// Created by neverchanje on 8/24/15.
//

#include "status.h"

#include <iostream>

class Task {
public:
    Status perform() { return Status::OK(); };
    Status perform2() { return Status::Failed(); }
};

int main() {
    Task task;

    Status stat = task.perform();
    if(stat.isOK() ) {
        std::cout<<stat.toString()<<std::endl;
    }

    stat = task.perform2();
    if(stat.isFailed() ) {
        std::cout<<stat.toString()<<std::endl;
    }
}
