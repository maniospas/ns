#include "Fallfront.h"
#include "Scope.h"
#include <iostream>

Fallfront::Fallfront(std::shared_ptr<Object> object, std::shared_ptr<Object> name): object_(object), name_(name) {
}

Fallfront::~Fallfront() {
}

const std::string Fallfront::name() const {
    return object_->name()+"#"+name_->name();
}

std::shared_ptr<Object> Fallfront::value(std::shared_ptr<Scope> scope) {
    push();
    auto object = object_->value(scope);
    std::shared_ptr<Scope> entered = std::dynamic_pointer_cast<Scope>(object);
    exists(entered);
    auto enchanced = std::make_shared<Scope>(entered, 
                                             entered,
                                             std::shared_ptr<Scope>(nullptr),
                                             std::shared_ptr<Scope>(nullptr),
                                             std::shared_ptr<Scope>(nullptr),
                                             scope);//scope is the fallfront
    
    return pop(name_->value(enchanced));
}
