#include "Assign.h"
#include "Scope.h"

Assign::Assign(std::shared_ptr<Object> name, std::shared_ptr<Object> object): name_(name), object_(object) {
}

Assign::~Assign() {
}

const std::string Assign::name() const {
    return name_->name()+"="+object_->name();
}

std::shared_ptr<Object> Assign::value(std::shared_ptr<Scope> scope) {
    auto key = name_;//->value(scope);
    auto value = object_->value(scope);
    scope->set(key->assignment_name(), value);
    return value;
}
