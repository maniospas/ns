#include "Assign.h"
#include "Scope.h"

Assign::Assign(std::shared_ptr<Object> name, std::shared_ptr<Object> object): name_(name), object_(object) {
}

Assign::~Assign() {
}

const std::string Assign::name() const {
    std::string ret("");
    if(name_!=nullptr)
        ret += name_->name();
    ret += "=";
    if(object_!=nullptr)
        ret += object_->name();
    return ret;
}

std::shared_ptr<Object> Assign::value(std::shared_ptr<Scope> scope) {
    push(scope);
    auto key = name_;//->value(scope);
    auto value = exists(scope, exists(scope, object_, "assignment expression ")->value(scope), "assignment name");
    scope->set(key->assignment_name(), value);
    return pop(scope, value);
}
