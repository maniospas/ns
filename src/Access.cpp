#include "Access.h"
#include "Scope.h"
#include <iostream>

Access::Access(std::shared_ptr<Object> object, std::shared_ptr<Object> name): object_(object), name_(name) {
}

Access::~Access() {
}

const std::string Access::name() const {
    std::string ret("");
    if(object_!=nullptr)
        ret += object_->name();
    ret += ":";
    if(name_!=nullptr)
        ret += name_->name();
    return ret;
}

std::shared_ptr<Object> Access::value(std::shared_ptr<Scope> scope) {
    exists(object_, "expression generating the object to access");
    auto object = object_->value(scope);
    exists(object, "object to access");
    object->lock(scope->owner); // prevent concurrent access to the same base object/scope
    try {
        std::shared_ptr<Scope> object_scope = std::dynamic_pointer_cast<Scope>(object);
        if(object_scope==nullptr)
            object_scope = std::make_shared<Scope>(scope, object, scope, scope->owner);
        //else 
        //    object_scope->conclude_threads();

        std::shared_ptr<Scope> entered = std::make_shared<Scope>(object, object, scope, scope->owner);
        auto ret = exists(name_, "accessing expression")->value(entered);
        object->unlock(scope->owner);
        return ret;
    }
    catch (std::runtime_error& e) {
        object->unlock(scope->owner);
        throw e;
    }
}
