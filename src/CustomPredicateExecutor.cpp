#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include "String.h"
#include <iostream>
#include "parser.h"
#include "Number.h"
#include "Scoped.h"
#include "Unscoped.h"


CustomPredicateExecutor::CustomPredicateExecutor(std::vector<std::shared_ptr<PredicatePart>> names): names_(names) {
}

CustomPredicateExecutor::CustomPredicateExecutor(const std::string& text): names_(predicate_parts(tokenize(text))) {
}

CustomPredicateExecutor::~CustomPredicateExecutor() {
}

static std::vector<std::shared_ptr<PredicatePart>> non_matching_parts;


std::shared_ptr<Object> create_predicate_or_primitive(std::vector<std::shared_ptr<PredicatePart>>& gathered) {
    if(gathered.size()==1 
        && gathered[0]->object()!=nullptr 
        && (
             std::dynamic_pointer_cast<Number>(gathered[0]->object())!=nullptr
             || std::dynamic_pointer_cast<String>(gathered[0]->object())!=nullptr
             || std::dynamic_pointer_cast<Scoped>(gathered[0]->object())!=nullptr
             || std::dynamic_pointer_cast<Unscoped>(gathered[0]->object())!=nullptr
        ))
            return gathered[0]->object();
    return std::make_shared<Predicate>(gathered);
}

std::vector<std::shared_ptr<PredicatePart>> CustomPredicateExecutor::match(std::vector<std::shared_ptr<PredicatePart>> names) {
    std::vector<std::shared_ptr<PredicatePart>> ret;
    int i = 0;
    //std::cout<<"checking "<<name()<<"\n";
    for(int pos=0;pos<names_.size();pos++) {
        if(i>=names.size())
            return non_matching_parts;
        if(names_[pos]->object()==nullptr) {
            if(names[i]->name()==names_[pos]->name()) {
                ret.push_back(names[i]);
                i++;
            }
            else 
                return non_matching_parts;
        }
        else if(pos==names_.size()-1) {
            std::vector<std::shared_ptr<PredicatePart>> gathered;
            while(i<names.size()) {
                gathered.push_back(names[i]);
                i++;
            }
            ret.push_back(std::make_shared<PredicatePartObject>(create_predicate_or_primitive(gathered)));
        }
        else if(names_[pos+1]->object()==nullptr) {
            std::vector<std::shared_ptr<PredicatePart>> gathered;
            while(i<names.size() && names_[pos+1]->name()!=names[i]->name()) {
                gathered.push_back(names[i]);
                i++;
            }
            ret.push_back(std::make_shared<PredicatePartObject>(create_predicate_or_primitive(gathered)));
        }
        else
            return non_matching_parts;
    }
    if(i<names.size()-1)
        return non_matching_parts;
    return ret;
}

const std::string CustomPredicateExecutor::name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->name();
    }
    return name;
}

const std::string CustomPredicateExecutor::assignment_name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->assignment_name();
    }
    return name;
}

std::shared_ptr<Object> CustomPredicateExecutor::value(std::shared_ptr<Scope> scope) {
    scope->load(std::dynamic_pointer_cast<CustomPredicateExecutor>(shared_from_this()));
    return shared_from_this();
}

class PredicateScope: public Scope {
    std::shared_ptr<Object> get(const std::string& name) {
        return std::make_shared<String>(name);  // this class returns the string variable name as the variable value
    }
};

std::shared_ptr<Scope> CustomPredicateExecutor::scoped(std::shared_ptr<Scope> scope) {
    return scope->enter();
}

std::shared_ptr<Scope> CustomPredicateExecutor::descoped(std::shared_ptr<Scope> derived_scope) {
    derived_scope->set("new", derived_scope->get("super"));
    //derived_scope->set("fallfront", std::shared_ptr<Scope>(nullptr));
    return derived_scope;
}

std::shared_ptr<Scope> CustomPredicateExecutor::vscoped(std::shared_ptr<Scope> scope) {
    return scope;
}

std::shared_ptr<Scope> CustomPredicateExecutor::predicateScope = std::make_shared<PredicateScope>();


void CustomPredicateExecutor::evaluate_argument(
    std::shared_ptr<Scope> derived_scope,
    std::shared_ptr<Object> prototype, 
    std::shared_ptr<Object> expression, 
    std::shared_ptr<Scope> value_scope) {
    if(std::dynamic_pointer_cast<Unscoped>(prototype)!=nullptr)
        prototype = exists(std::dynamic_pointer_cast<Unscoped>(prototype)->contents());
    if(std::dynamic_pointer_cast<Unscoped>(prototype)!=nullptr){
        std::string name = exists(exists(std::dynamic_pointer_cast<Unscoped>(prototype)->contents())->value(CustomPredicateExecutor::predicateScope))->name();
        derived_scope->set(name, expression);
    }
    else
        derived_scope->set(prototype->value(CustomPredicateExecutor::predicateScope)->name(), 
                            exists(expression->value(value_scope)));
}

std::shared_ptr<Object> CustomPredicateExecutor::call(std::shared_ptr<Scope> scope, std::shared_ptr<Predicate> predicate) {
    auto prev_scope = scope;
    auto value_scope = vscoped(scope);
    auto derived_scope = scoped(scope);
    int N = names_.size();
    if(predicate!=nullptr && names_.size()==predicate->names_.size())
        for(int i=0;i<N;i++) {
            auto expression = predicate->names_[i]->object();
            if(expression!=nullptr)
                evaluate_argument(derived_scope, names_[i]->object(), expression, value_scope);
        }
    derived_scope = descoped(derived_scope);
    return implement(derived_scope);
}

