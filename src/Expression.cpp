#include "Expression.h"
#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include "parser.h"
#include "Number.h"
#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <algorithm>


Expression::Expression(std::vector<std::shared_ptr<PredicatePart>> names): names_(names) {
    for(int i=0;i<names_.size();i++)  {
        if(names_[i]==nullptr)
            error("Empty predicate part");
    }
}

Expression::~Expression() {
}

const std::string Expression::name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->name();
    }
    return name;
}

bool sorter (std::shared_ptr<CustomPredicateExecutor> i, std::shared_ptr<CustomPredicateExecutor> j) { 
    if(i->priority == j->priority)
        return i->name().length()<j->name().length();
    return (i->priority<j->priority); 
}

std::shared_ptr<Object> Expression::value(std::shared_ptr<Scope> scope) {
    push();
    auto obj = scope->get(assignment_name());
    std::shared_ptr<CustomPredicateExecutor> executor = std::dynamic_pointer_cast<CustomPredicateExecutor>(obj);
    if(obj!=nullptr && executor==nullptr)
        return pop(obj);
    if(obj==nullptr) {
        // just in time parsing based on parent scope
        std::map<std::string, std::shared_ptr<CustomPredicateExecutor>>executors_map;
        scope->gather_executors(executors_map);
        int max_length = 0;
        int max_priority = -1000;
        int first_pos = names_.size();
        std::vector<std::shared_ptr<CustomPredicateExecutor>> executors;
        for (const auto &it : executors_map)
            executors.push_back(it.second); 
        //std::sort(executors.begin(), executors.end(), sorter);
    
        for(const auto& exec : executors) {
            //if(exec->priority < max_priority)
            //    continue;
            //std::cout << exec->name() << "\n";
            int pos = (exec->priority > max_priority)?names_.size()+1:first_pos;
            std::vector<std::shared_ptr<PredicatePart>> names = exec->match(names_, pos);
            //std::cout << exec->name() << pos << "\n";
            //std::cout << name() << "\n";
            if(names.size()==0)
                continue;
            if((exec->priority > max_priority) 
                || ((exec->priority == max_priority) 
                    && (pos<first_pos 
                        || (pos==first_pos && names.size()>max_length)))) {
                names_.assign(names.begin(), names.end());
                executor = exec;
                max_length = names.size();
                first_pos = pos;
                max_priority = exec->priority;
                //break;
            }
        }
        /*if(max_length>0)
            for(int i=0;i<names_.size();i++)
                if(names_[i]->object()==nullptr && is_number_convertible(names_[i]->name()))
                    names_[i] = std::make_shared<PredicatePartObject>(std::make_shared<Number>(std::stof(names_[i]->name())));
        */      

    }
    exists(executor);
    auto value_scope = executor->evaluate_all_arguments(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()));
    if(executor->can_call(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()), value_scope)){
        return pop(executor->call(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()), value_scope));
    }
    auto overloaded_versions = std::vector<std::shared_ptr<CustomPredicateExecutor>>();
    scope->gather_overloads(assignment_name(), overloaded_versions);
    for(auto const& version : overloaded_versions) {
        auto exec = std::dynamic_pointer_cast<CustomPredicateExecutor>(version);
        if(exec==nullptr || exec==executor)
            continue; // do not match with variables, we have already checked that the executor does not fit
        // TODO: for HUGE speedup, create arguments as lists and only alignt the evaluations here to create a new scope
        value_scope = exec->evaluate_all_arguments(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()));
        if(exec->can_call(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()), value_scope))
          return pop(exec->call(scope, std::dynamic_pointer_cast<Expression>(shared_from_this()), value_scope));
    }
    error("No implementation of "+executor->assignment_name()+" matches argument constraints");
    return pop(std::shared_ptr<Object>(nullptr));
}

const std::string Expression::assignment_name() const {
    std::string name = "";
    for(int i=0;i<names_.size();i++) {
        if(i)
            name += " ";
        name += names_[i]->assignment_name();
    }
    return name;
}
