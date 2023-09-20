#include "Scoped.h"
#include "Scope.h"
#include <iostream>


Scoped::Scoped(std::shared_ptr<Object> object) : object_(object) {
}

Scoped::~Scoped() {
}

const std::string Scoped::name() const {
    if(object_==nullptr)
        return "{}";
    return "{"+object_->name()+"}";
}

std::shared_ptr<Object> Scoped::value(std::shared_ptr<Scope> scope) {
    object_->lock(scope->owner);
    try {
        push(scope);
        std::shared_ptr<Scope> entered = scope->enter();
        auto ret = pop(scope, exists(scope, object_, "scope contents")->value(entered));
        object_->unlock(scope->owner);
        return ret;
    }
    catch (std::runtime_error& e) {
        object_->unlock(scope->owner);
        throw e;
    }
}
