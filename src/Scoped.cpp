#include "Scoped.h"
#include "Scope.h"

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
    push();
    std::shared_ptr<Scope> entered = scope->enter();
    

    return pop(exists(object_)->value(entered));
}
