#ifndef SCOPE_H
#define SCOPE_H

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "Object.h"
#include "Thread.h"
#include <pthread.h>
#include <iostream>

class CustomPredicateExecutor;

class Scope: public Object {
    private:
        pthread_mutex_t entry_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t access_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
        std::map<std::string, std::shared_ptr<Object>> values;
        std::map<std::string, std::unique_ptr<std::vector<std::shared_ptr<Object>>>> overloaded;
        int thread_depth = 0;
    public:
        std::shared_ptr<Thread> owner;
        std::vector<std::shared_ptr<Thread>> threads;
        Scope(const std::shared_ptr<Object> super_, 
              const std::shared_ptr<Object> new_, 
              const std::shared_ptr<Object> surface_,
              const std::shared_ptr<Thread> owner_);
        virtual ~Scope();
        void set(const std::string& name, const std::shared_ptr<Object> value);
        std::shared_ptr<Object> get(const std::string& name);
        void gather_overloads(const std::string& signature, std::vector<std::shared_ptr<CustomPredicateExecutor>>& overloads);
        std::shared_ptr<Scope> enter();
        int size() const {return 0;};
        const std::string name() const;
        void gather_executors(std::map<std::string, std::shared_ptr<CustomPredicateExecutor>>& executors);
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        void load(std::shared_ptr<Object> method);
        const std::string type() const {return "scope";};
        void conclude_threads();
        void lock(const std::shared_ptr<Thread> thread);
        void unlock(const std::shared_ptr<Thread> thread);
        void access_lock(const std::shared_ptr<Thread> thread);
        void access_unlock(const std::shared_ptr<Thread> thread);
        void thread_lock();
        void thread_unlock();
};

#endif
