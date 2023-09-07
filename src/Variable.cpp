#include "Variable.h"
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include <memory>
#include <iostream>

Variable::Variable(const std::string& name): name_(name) {
}

Variable::~Variable() {
}

const std::string Variable::name() const {
    return name_;
}

std::shared_ptr<Object> Variable::value(std::shared_ptr<Scope> scope) {
    return scope->get(name_);
    //std::cout << scope->get("["+name_+"]") << "\n";
    // move on to having parsed an one-word predicate without variables
    //return std::dynamic_pointer_cast<CustomPredicateExecutor>(scope->get("["+name_+"]"))->call(scope, std::shared_ptr<Expression>(nullptr));
}
