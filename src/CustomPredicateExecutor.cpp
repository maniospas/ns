#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include "PString.h"
#include <iostream>
#include "parser.h"
#include "Number.h"
#include "Access.h"
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
        && gathered[0]->object()->is_parsed())
            return gathered[0]->object();
    return std::make_shared<Expression>(gathered);
}

std::vector<std::shared_ptr<PredicatePart>> CustomPredicateExecutor::match(
    std::vector<std::shared_ptr<PredicatePart>>& names,
    int& first_predicate_position) {
    std::vector<std::shared_ptr<PredicatePart>> ret;
    int i = 0;
    int initial_first_predicate_position = first_predicate_position;
    //std::cout<<"checking "<<name()<<"\n";
    for(int pos=0;pos<names_.size();pos++) {
        if(i>=names.size() || i>initial_first_predicate_position) // last condition prevents on average half of expressions from taking more time
            return non_matching_parts;
        if(names_[pos]->object()==nullptr) {
            if(names[i]->name()==names_[pos]->name()) {
                ret.push_back(names[i]);
                if(i<first_predicate_position)
                    first_predicate_position = i;
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
    auto shared_this = shared_from_this();
    scope->load(shared_this);
    return shared_this;
}

class PredicateScope: public Scope {
    public:
        PredicateScope() : Scope(std::shared_ptr<Object>(nullptr), std::shared_ptr<Object>(nullptr), std::shared_ptr<Object>(nullptr), std::shared_ptr<Thread>(nullptr)) {
        }
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
        prototype = exists(std::dynamic_pointer_cast<Unscoped>(prototype)->contents(), "empty argument definition");
    if(std::dynamic_pointer_cast<Unscoped>(prototype)!=nullptr){
        std::string name = exists(exists(std::dynamic_pointer_cast<Unscoped>(prototype)->contents(), "empty lambda argument name")->value(CustomPredicateExecutor::predicateScope), "empty lambda argument evaluated name")->name();
        derived_scope->set(name, expression);
    }
    else {
        auto value = expression->value(value_scope);
        auto edit = std::dynamic_pointer_cast<Access>(prototype);
        if(edit!=nullptr)
            prototype = exists(edit->object(), "argument condition");
        if(prototype==nullptr)
            error("Callable "+edit->name()+"has no accessor.");
        auto prototype_value = prototype->value(CustomPredicateExecutor::predicateScope);
        if(prototype_value==nullptr)
            error("No argument value for "+prototype->name());
        std::string assign_name = prototype_value->name();
        derived_scope->set(assign_name, value);
    }
}


std::shared_ptr<Scope> CustomPredicateExecutor::evaluate_all_arguments(std::shared_ptr<Scope> scope, std::shared_ptr<Expression> predicate) {
    
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
    return derived_scope;
}


bool CustomPredicateExecutor::can_call(std::shared_ptr<Scope> scope, std::shared_ptr<Expression> predicate, std::shared_ptr<Scope> derived_scope) {
    for(const auto& name : names_) {
        auto prototype = name->object();
        if(prototype==nullptr)
            continue;
        if(std::dynamic_pointer_cast<Unscoped>(prototype)!=nullptr)
            prototype = std::dynamic_pointer_cast<Unscoped>(prototype)->contents();
        auto edit = std::dynamic_pointer_cast<Access>(prototype);
        if(edit==nullptr) 
            continue;
        prototype = edit->object();
        if(prototype==nullptr)
            error("Missing name in conditional argument declaration (empty name before :).");
        std::string assign_name = prototype->value(CustomPredicateExecutor::predicateScope)->name();
        auto prototype_object = derived_scope->get(assign_name);
        if(prototype_object==nullptr)
            error("Argument '"+assign_name+"' not found in the argument declaration's evaluation scope (this shouldn't happen).");
        auto entered = std::make_shared<Scope>(prototype_object, std::shared_ptr<Object>(nullptr), std::shared_ptr<Object>(nullptr), scope->owner);
        auto condition = edit->expression();
        if(condition==nullptr) 
            error("Missing condition in argument declaration (empty expression after :, use unconditional declaration).");
        auto valid = condition->value(entered);
        if(valid==nullptr)
            return false;
            //error("Argument condition returned None (should return either true of false).");
        auto number = std::dynamic_pointer_cast<Number>(valid);
        if(number==nullptr || !number->value())
            return false;//error("Argument condition not satisfied due to return 0.");
    }
    return true;
}

std::shared_ptr<Object> CustomPredicateExecutor::call(std::shared_ptr<Scope> scope, std::shared_ptr<Expression> predicate, std::shared_ptr<Scope> derived_scope) {
    return implement(derived_scope);
}

