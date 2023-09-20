#ifndef THREAD_H
#define THREAD_H

#include <string>
#include "Object.h"
#include <pthread.h>

class Thread: public Object
{
    private:
        pthread_t thread;
    public:
        std::shared_ptr<Object> result;
        Thread();
        virtual ~Thread();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        pthread_t* value();
        const std::string type() const {return "thread";};
        bool is_primitive() {return true;};
        bool is_parsed() {return true;};
        std::string get_stack_trace();
        std::list<std::shared_ptr<Object>> stack;
};

#endif
