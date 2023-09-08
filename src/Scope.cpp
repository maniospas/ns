#include <iostream>
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "parser.h"



Scope::Scope(const std::shared_ptr<Object> super_, 
    const std::shared_ptr<Object> new_,
    const std::shared_ptr<Object> surface_
    ) {
    values["super"] = super_;
    values["new"] = new_;
    values["surface"] = surface_;
}

Scope::~Scope() {
}

void Scope::gather_executors(std::map<std::string, std::shared_ptr<CustomPredicateExecutor>>& executors) {
    auto surfacescope = values["surface"];
    if(surfacescope!=nullptr && std::dynamic_pointer_cast<Scope>(surfacescope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(surfacescope)->gather_executors(executors);
    std::shared_ptr<CustomPredicateExecutor> casted;
    for(auto it=values.begin();it!=values.end();++it) 
        if(it->second!=nullptr 
        && (casted=std::dynamic_pointer_cast<CustomPredicateExecutor>(it->second))!=nullptr
        && executors.find(it->first)==executors.end())
            executors[it->first] = casted;
    auto superscope = values["super"];
    if(superscope!=nullptr && std::dynamic_pointer_cast<Scope>(superscope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(superscope)->gather_executors(executors);
}


void Scope::gather_overloads(const std::string& signature, std::vector<std::shared_ptr<CustomPredicateExecutor>>& overloads) {
    auto surfacescope = values["surface"];
    if(surfacescope!=nullptr && std::dynamic_pointer_cast<Scope>(surfacescope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(surfacescope)->gather_overloads(signature, overloads);
    
    std::shared_ptr<CustomPredicateExecutor> casted;
    auto it = overloaded.find(signature);
    if(it!=overloaded.end()) {
        //auto N = overloads.size();
        //overloads.reserve(N+it->second->size());
        for(const auto& second : *it->second)
            if((casted=std::dynamic_pointer_cast<CustomPredicateExecutor>(second))!=nullptr
                && it->first == signature) {
                overloads.push_back(casted);
                //N++;
            }
    }

    auto superscope = values["super"];
    if(superscope!=nullptr && std::dynamic_pointer_cast<Scope>(superscope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(superscope)->gather_overloads(signature, overloads);
}

void Scope::set(const std::string& name, const std::shared_ptr<Object> value) {
    if(name!="new" && values["new"]!=nullptr)
        values["new"]->set(name, value);
    else if(value==nullptr) 
        values.erase(name);
    else {
        values[name] = value;
        auto edit = std::dynamic_pointer_cast<CustomPredicateExecutor>(value);
        if(edit!=nullptr) {
            auto it = overloaded.find(name);
            if(it==overloaded.end()) {
                overloaded[name] = std::make_unique<std::vector<std::shared_ptr<Object>>>();
                overloaded[name]->push_back(value);
            }
            else
                it->second->push_back(value);
        }
    }
}

std::shared_ptr<Object> Scope::get(const std::string& name) {
    /*auto surface_ = values.find("surface");
    if(surface_!=values.end() && surface_->second!=nullptr)
    {
        std::shared_ptr<Object> ret = surface_->second->get(name);
        if(ret!=nullptr) 
            return ret;
    }*/

    /*auto new_ = values.find("new");
    if(new_!=values.end() && new_->second!=nullptr) {
        return new_->second->get(name);
    }*/
    if(name=="new")// && values["new"] == nullptr)
        return shared_from_this();
    if(name=="super" && values["super"]==values["new"] && values["super"]!=nullptr) 
        return values["super"]->get("super");
    auto it = values.find(name);
    if(it==values.end()) {
        /*auto new_ = values.find("new");
        if(new_!=values.end() && new_->second!=nullptr && (ret=new_->second->get(name))!=nullptr)
            return ret;*/
        auto parent_ = values.find("super");
        if(parent_!=values.end() && parent_->second!=nullptr) {
            std::shared_ptr<Object> ret = parent_->second->get(name);
            if(ret!=nullptr)
                return ret;
        }
        return std::shared_ptr<Object>(nullptr);
    }
    return it->second;
}

std::shared_ptr<Scope> Scope::enter() {
    return std::make_shared<Scope>(shared_from_this(), std::shared_ptr<Object>(nullptr), std::shared_ptr<Object>(nullptr));
}

const std::string Scope::name() const {
    std::string desc = "{";
    for(auto it=values.begin(); it!=values.end();++it) {
        //if(it->first=="super" && it->second!=nullptr && (it->second.get())==values["new"].get()) 
        //    continue;
      /*if(it->first=="new") {
        if(it->second!=nullptr)
            desc += "("+it->second->name().substr(1, -1)+");";
        continue;
      }*/
      if(it->first[0]!='[') {
          desc += it->first;
          desc += "=";
      }
      if(it->second!=nullptr && std::dynamic_pointer_cast<Scope>(it->second)!=nullptr)
        desc += "...";
      else if(it->second!=nullptr)
        desc += it->second->name();
      else
        desc += "None";
      desc += ";";
    }
    return desc+"new}";
}


std::shared_ptr<Object> Scope::value(std::shared_ptr<Scope> scope) {
    return shared_from_this();
}

void Scope::load(std::shared_ptr<Object> method) {
    set(method->assignment_name(), method);
}
