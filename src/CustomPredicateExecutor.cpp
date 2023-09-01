#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include "String.h"
#include <iostream>
#include "parser.h"

CustomPredicateExecutor::CustomPredicateExecutor(std::vector<std::shared_ptr<PredicatePart>> names): names_(names) {
}

CustomPredicateExecutor::CustomPredicateExecutor(const std::string& text): names_(predicate_parts(tokenize(text))) {
}

CustomPredicateExecutor::~CustomPredicateExecutor() {
}

static std::vector<std::shared_ptr<PredicatePart>> non_matching_parts;

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
            ret.push_back(std::make_shared<PredicatePartObject>(std::make_shared<Predicate>(gathered)));
        }
        else if(names_[pos+1]->object()==nullptr) {
            std::vector<std::shared_ptr<PredicatePart>> gathered;
            while(i<names.size() && names_[pos+1]->name()!=names[i]->name()) {
                gathered.push_back(names[i]);
                i++;
            }
            ret.push_back(std::make_shared<PredicatePartObject>(std::make_shared<Predicate>(gathered)));
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

std::shared_ptr<Object> CustomPredicateExecutor::evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope) {
    return exists(expression->value(scope));
}

std::shared_ptr<Object> CustomPredicateExecutor::call(std::shared_ptr<Scope> scope, std::shared_ptr<Predicate> predicate) {
    auto prev_scope = scope;
    auto value_scope = vscoped(scope);
    auto derived_scope = scoped(scope);
    int N = names_.size();
    if(predicate!=nullptr && names_.size()==predicate->names_.size())
        for(int i=0;i<N;i++) {
            auto expression = predicate->names_[i]->object();
            if(expression!=nullptr){
                derived_scope->set(names_[i]->object()->value(CustomPredicateExecutor::predicateScope)->name(), evaluate_argument(expression, value_scope));
            }
        }
    derived_scope = descoped(derived_scope);
    return implement(derived_scope);
}

