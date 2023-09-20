#include <iostream>
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "parser.h"



Scope::Scope(const std::shared_ptr<Object> super_, 
    const std::shared_ptr<Object> new_,
    const std::shared_ptr<Object> surface_,
    const std::shared_ptr<Thread> owner_
    ) {
    //if(super_!=nullptr)
        values["super"] = super_;
    //if(new_!=nullptr)
        values["new"] = new_;
    //if(surface_!=nullptr)
        values["ext"] = surface_;
    owner = owner_;
}

Scope::~Scope() {
}


void Scope::lock(const std::shared_ptr<Thread> thread){
    auto newscope = values["new"];
    if(newscope!=nullptr) {
        newscope->lock(thread);
        return;
    }
    if(owner.get()==thread.get() && thread_depth) {
        thread_depth += 1;
        //std::cout<<"already locked "<<this<<"\n";
        return;
    }
    pthread_mutex_lock(&entry_mutex);
    owner = thread;
    thread_depth = 1;
    //std::cout<<"locked "<<this<<"\n";
    auto superscope = values["super"];
    if(superscope!=nullptr) 
        superscope->lock(thread);
}

void Scope::unlock(const std::shared_ptr<Thread> thread){
    auto newscope = values["new"];
    if(newscope!=nullptr) {
        newscope->unlock(thread);
        return;
    }
    auto superscope = values["super"];
    if(superscope!=nullptr) 
        superscope->unlock(thread);
    if(thread.get()!=owner.get())
        error(std::dynamic_pointer_cast<Scope>(shared_from_this()), "Synchronization error on unlocking scope from different thread.");
    thread_depth -= 1;
    if(thread_depth==0){
        //std::cout<<"unlocked "<<this<<"\n";
        owner = std::shared_ptr<Thread>(nullptr);
    }
    pthread_mutex_unlock(&entry_mutex);
}

void Scope::access_lock(const std::shared_ptr<Thread> thread){
    //if(thread!=owner)
    //    pthread_mutex_lock(&access_mutex);
}

void Scope::access_unlock(const std::shared_ptr<Thread> thread){
    //if(thread!=owner)
    //    pthread_mutex_unlock(&access_mutex);
}


void Scope::thread_lock(){
    //pthread_mutex_lock(&thread_mutex);
}

void Scope::thread_unlock(){
    //pthread_mutex_unlock(&thread_mutex);
}

void Scope::gather_executors(std::map<std::string, std::shared_ptr<CustomPredicateExecutor>>& executors) {
    access_lock(owner);
    std::shared_ptr<CustomPredicateExecutor> casted;
    for(auto it=values.begin();it!=values.end();++it) 
        if(it->second!=nullptr 
        && (casted=std::dynamic_pointer_cast<CustomPredicateExecutor>(it->second))!=nullptr
        && executors.find(it->first)==executors.end())
            executors[it->first] = casted;
    
    auto superscope = values["super"];
    if(superscope!=nullptr && std::dynamic_pointer_cast<Scope>(superscope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(superscope)->gather_executors(executors);
    access_unlock(owner);
    
    /*auto surfacescope = values["ext"];
    if(surfacescope!=nullptr && std::dynamic_pointer_cast<Scope>(surfacescope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(surfacescope)->gather_executors(executors);*/
}


void Scope::conclude_threads() {
    thread_lock();
    // need to also conclude superthreads, because members can be accessed from there
    auto superscope = values["super"];
    if(superscope!=nullptr && std::dynamic_pointer_cast<Scope>(superscope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(superscope)->conclude_threads();
    for(const auto& thread : threads)
        if(thread.get()!=owner.get())
            pthread_join(*thread->value(), NULL);
    threads.clear();
    if(owner!=nullptr)
        threads.push_back(owner);
    thread_unlock();
}


void Scope::gather_overloads(const std::string& signature, std::vector<std::shared_ptr<CustomPredicateExecutor>>& overloads) {
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
    
    /*auto surfacescope = values["ext"];
    if(surfacescope!=nullptr && std::dynamic_pointer_cast<Scope>(surfacescope)!=nullptr)
        std::dynamic_pointer_cast<Scope>(surfacescope)->gather_overloads(signature, overloads);
    */
}


void Scope::set(const std::string& name, const std::shared_ptr<Object> value) {
    access_lock(owner);
    /*if(name=="new") {
        access_unlock(owner);
        error("Cannot set the new value of a scope");
    }
    else if(name=="super") {
        access_unlock(owner);
        error("Cannot set the super value of a scope");
    }
    else */
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
    access_unlock(owner);
}

std::shared_ptr<Object> Scope::get(const std::string& name) {
    /*auto surface_ = values.find("ext");
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
    access_lock(owner);
    if(name=="new" && values["new"] == nullptr) {
        access_unlock(owner);
        return shared_from_this();
    }
    if(name=="super" && values["super"]==values["new"] && values["super"]!=nullptr) {
        access_unlock(owner);
        return values["super"]->get("super");
    }
    auto it = values.find(name);
    if(it==values.end() || it->second==nullptr) {
        /*auto new_ = values.find("new");
        if(new_!=values.end() && new_->second!=nullptr && (ret=new_->second->get(name))!=nullptr)
            return ret;*/
        auto parent_ = values.find("super");
        if(parent_!=values.end() && parent_->second!=nullptr) {
            std::shared_ptr<Object> ret = parent_->second->get(name);
            if(ret!=nullptr) {
                access_unlock(owner);
                return ret;
            }
        }
        /*auto ext_ = values.find("ext");
        if(ext_!=values.end() && ext_->second!=nullptr) {
            std::shared_ptr<Object> ret = ext_->second->get(name);
            if(ret!=nullptr)
                return ret;
        }*/
        return std::shared_ptr<Object>(nullptr);
    }
    access_unlock(owner);
    return it->second;
}

std::shared_ptr<Scope> Scope::enter() {
    return std::make_shared<Scope>(shared_from_this(), std::shared_ptr<Object>(nullptr), std::shared_ptr<Object>(nullptr), owner);
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
