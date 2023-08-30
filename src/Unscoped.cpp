#include "Unscoped.h"
#include "Scope.h"

Unscoped::Unscoped(std::shared_ptr<Object> object) : object_(object) {
}

Unscoped::~Unscoped() {
}

const std::string Unscoped::name() const {
    return "("+object_->name()+")";
}

std::shared_ptr<Object> Unscoped::value(std::shared_ptr<Scope> scope) {
    return object_->value(scope);
}
