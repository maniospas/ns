#include "Edit.h"
#include "Scope.h"
#include <iostream>

Edit::Edit(std::shared_ptr<Object> object, std::shared_ptr<Object> name): object_(object), name_(name) {
}

Edit::~Edit() {
}

const std::string Edit::name() const {
    return object_->name()+":"+name_->name();
}

std::shared_ptr<Object> Edit::value(std::shared_ptr<Scope> scope) {
    push();
    auto object = object_->value(scope);
    std::shared_ptr<Scope> entered = std::dynamic_pointer_cast<Scope>(object);
    exists(entered);
    auto enchanced = std::make_shared<Scope>(entered, 
                                             entered,
                                             std::shared_ptr<Scope>(nullptr),
                                             scope);//scope is the fallback
    
    return pop(name_->value(enchanced));
}
