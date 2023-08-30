#include <iostream>
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "parser.h"



Scope::Scope(const std::shared_ptr<Object> parent, const std::shared_ptr<Object> surface, const std::shared_ptr<Scope> prototype) {
    if(prototype!=nullptr)
        values = prototype->values;
    values["super"] = parent;
    values["new"] = surface;
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

std::shared_ptr<Object> Scope::get(const std::string& name) {
    if(name=="new")
        return shared_from_this();
    auto it = values.find(name);
    if(it==values.end()) {
        auto parent_ = values.find("super");
        if(parent_!=values.end() && parent_->second!=nullptr)
            return parent_->second->get(name);
        error("Variable does not exist: "+name);
        //return std::shared_ptr<Object>(nullptr);
    }
    return it->second;
}

std::shared_ptr<Scope> Scope::enter() {
    return std::make_shared<Scope>(shared_from_this());
}

const std::string Scope::name() const {
    std::string desc = "{";
    for(auto it=values.begin(); it!=values.end();++it) {
      if(it->first=="super")
        continue;
      if(it->first=="new") {
        if(it->second!=nullptr)
            desc += it->second->name().substr(1, -1)+";";
        continue;
      }
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
