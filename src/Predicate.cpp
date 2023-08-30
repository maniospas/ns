#include "Predicate.h"
#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include <iostream>


Predicate::Predicate(std::vector<std::shared_ptr<PredicatePart>> names): names_(names) {
    //std::cout << name() << "\n";
}

Predicate::~Predicate() {
}

const std::string Predicate::name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->name();
    }
    return name;
}

std::shared_ptr<Object> Predicate::value(std::shared_ptr<Scope> scope) {
    push();
    auto obj = exists(scope->get(assignment_name()));
    std::shared_ptr<CustomPredicateExecutor> executor = std::dynamic_pointer_cast<CustomPredicateExecutor>(obj);
    if(executor==nullptr)
        return pop(obj);
    return pop(executor->call(scope, std::dynamic_pointer_cast<Predicate>(shared_from_this())));
}

const std::string Predicate::assignment_name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->assignment_name();
    }
    return name;
}
