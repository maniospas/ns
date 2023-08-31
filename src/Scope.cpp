#include <iostream>
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "parser.h"



Scope::Scope(const std::shared_ptr<Object> parent, 
    const std::shared_ptr<Object> surface, 
    const std::shared_ptr<Scope> prototype,
    const std::shared_ptr<Scope> fallback,
    const std::shared_ptr<Scope> failback,
    const std::shared_ptr<Scope> fallfront
    ) {
    values["new"] = surface;
    values["super"] = parent;
    if(prototype!=nullptr)
        values = prototype->values;
    values["fallback"] = fallback;
    values["failback"] = failback;
    values["fallfront"] = fallfront;
}

Scope::~Scope() {
}

void Scope::set(const std::string& name, const std::shared_ptr<Object> value) {
    if(name!="new" && values["new"]!=nullptr)
        values["new"]->set(name, value);
    else if(value==nullptr)
        values.erase(name);
    else
        values[name] = value;
}

std::shared_ptr<Object> Scope::overlap(const std::string& name, const std::shared_ptr<Object> value) {
    if(values["failback"]!=nullptr) {
        std::shared_ptr<Object> ret = values["failback"]->get(name);
        if(ret!=nullptr) {
            if(value==nullptr)
                return ret;
            if(ret.get()!=value.get())
                error("Variable value conflict between entrant and accessed (.) scope: disambiguate with fallback(:) or fallfront(#)");
        }
    }
    return value;
}

std::shared_ptr<Object> Scope::get(const std::string& name) {
    if(name=="new")// && values["new"] == nullptr)
        return shared_from_this();
    if(values["fallfront"]!=nullptr) {
        std::shared_ptr<Object> ret = values["fallfront"]->get(name);
        if(ret!=nullptr) 
            return ret;
    }
    auto it = values.find(name);
    if(it==values.end()) {
        std::shared_ptr<Object> ret;
        /*auto new_ = values.find("new");
        if(new_!=values.end() && new_->second!=nullptr && (ret=new_->second->get(name))!=nullptr)
            return ret;*/
        auto parent_ = values.find("super");
        if(parent_!=values.end() && parent_->second!=nullptr && (ret=parent_->second->get(name))!=nullptr)
            return overlap(name, ret);
        auto fallback_ = values.find("fallback");
        if(fallback_!=values.end() && fallback_->second!=nullptr && (ret=fallback_->second->get(name))!=nullptr)
            return overlap(name, ret);
        return overlap(name, std::shared_ptr<Object>(nullptr));
    }
    return overlap(name, it->second);
}

std::shared_ptr<Scope> Scope::enter() {
    return std::make_shared<Scope>(shared_from_this());
}

const std::string Scope::name() const {
    std::string desc = "{";
    for(auto it=values.begin(); it!=values.end();++it) {
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

void Scope::load(std::shared_ptr<CustomPredicateExecutor> method) {
    set(method->assignment_name(), method);
}
