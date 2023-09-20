#include "Object.h"
#include "Scope.h"
#include "parser.h"
#include "CustomPredicateExecutor.h"
#include <string>
#include <memory>
#include <iostream>
#include "PString.h"
#include <pthread.h>

class MutexHolder {
public:
    static pthread_mutex_t io_mutex;
};

pthread_mutex_t MutexHolder::io_mutex = PTHREAD_MUTEX_INITIALIZER;

class PrintExecutor : public CustomPredicateExecutor {
public:
    PrintExecutor() : CustomPredicateExecutor("print(text)") {
    }
    std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
        pthread_mutex_lock(&MutexHolder::io_mutex);
        try {
            auto text = exists(scope->get("text"), "text");
            std::cout << text->name() << std::endl;
            pthread_mutex_unlock(&MutexHolder::io_mutex);
            return text;
        }
        catch (std::runtime_error &e) {
            pthread_mutex_unlock(&MutexHolder::io_mutex);
            throw e;
        }
    }
};

class InputExecutor : public CustomPredicateExecutor {
public:
    InputExecutor() : CustomPredicateExecutor("read(prompt)") {
    }
    std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
        pthread_mutex_lock(&MutexHolder::io_mutex);
        try {
            std::cout << exists(scope->get("prompt"), "prompt")->name();
            std::string input;
            std::getline(std::cin, input);
            pthread_mutex_unlock(&MutexHolder::io_mutex);
            return std::make_shared<String>(input);
        }
        catch (std::runtime_error &e) {
            pthread_mutex_unlock(&MutexHolder::io_mutex);
            throw e;
        }
    }
};

extern "C" __declspec(dllexport) std::shared_ptr<Object> run(std::shared_ptr<Scope> scope) {
    scope->load(std::make_shared<PrintExecutor>());
    scope->load(std::make_shared<InputExecutor>());
    return scope;
}
