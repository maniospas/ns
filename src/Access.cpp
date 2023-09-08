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
    ret += ".";
    if(name_!=nullptr)
        ret += name_->name();
    return ret;
}

std::shared_ptr<Object> Access::value(std::shared_ptr<Scope> scope) {
    auto object = object_->value(scope);
    std::shared_ptr<Scope> entered = std::make_shared<Scope>(object, object, scope);
    return exists(name_, "accessing expression")->value(entered);
}
