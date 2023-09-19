#include "Thread.h"

Thread::Thread() {
}

Thread::~Thread() {
}

const std::string Thread::name() const {
    if(result!=nullptr)
        return "thread (finished, use result = join(...) to get the result)";
    return "thread (running, use result = join(...) to wait for the result)";
}

std::shared_ptr<Object> Thread::value(std::shared_ptr<Scope> scope) {
    return shared_from_this();
}

pthread_t* Thread::value() {
    return &thread;
}
