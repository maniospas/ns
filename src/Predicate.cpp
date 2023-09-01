#include "Predicate.h"
#include "CustomPredicateExecutor.h"
#include "Scope.h"
#include "parser.h"
#include "Number.h"
#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <algorithm>


Predicate::Predicate(std::vector<std::shared_ptr<PredicatePart>> names): names_(names) {
    for(int i=0;i<names_.size();i++)  {
        if(names_[i]==nullptr)
            error("Empty predicate part");
    }
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

bool sorter (std::shared_ptr<CustomPredicateExecutor> i, std::shared_ptr<CustomPredicateExecutor> j) { 
    if(i->priority == j->priority)
        return i->name().length()<j->name().length();
    return (i->priority<j->priority); 
}

std::shared_ptr<Object> Predicate::value(std::shared_ptr<Scope> scope) {
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
        std::vector<std::shared_ptr<CustomPredicateExecutor>> executors;
        for (const auto &it : executors_map)
            executors.push_back(it.second); 
        std::sort(executors.begin(), executors.end(), sorter);

        for(const auto& exec : executors) {
            std::vector<std::shared_ptr<PredicatePart>> names = exec->match(names_);
            if(names.size()>max_length) {
                names_.assign(names.begin(), names.end());
                executor = exec;
                max_length = executor->priority; //names.size();
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
