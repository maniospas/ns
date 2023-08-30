#include "Access.h"
#include "Scope.h"
#include <iostream>

Access::Access(std::shared_ptr<Object> object, std::shared_ptr<Object> name): object_(object), name_(name) {
}

Access::~Access() {
}

const std::string Access::name() const {
    return object_->name()+"."+name_->name();
}

std::shared_ptr<Object> Access::value(std::shared_ptr<Scope> scope) {
    auto object = object_->value(scope);
    std::shared_ptr<Scope> entered = std::dynamic_pointer_cast<Scope>(object);
    return name_->value(entered);
}
