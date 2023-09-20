#include "Object.h"
#include "parser.h"
#include "Scope.h"


Object::Object() {
}


std::shared_ptr<Object> Object::safe_get(const std::string& name) {
    return exists(std::dynamic_pointer_cast<Scope>(shared_from_this()), get(name), name);
};

const std::string Object::assignment_name() const {
    return name();
}

std::shared_ptr<Object> Object::get(const std::string& name) {
    return std::shared_ptr<Object>(nullptr);
}

void Object::set(const std::string& name, const std::shared_ptr<Object> value) {
}

void Object::push(const std::shared_ptr<Scope>& scope) {
    if(scope==nullptr)
        return;
    if(scope->owner==nullptr)
        return;
    scope->owner->stack.push_back(shared_from_this());
}

std::shared_ptr<Object> Object::exists(const std::shared_ptr<Scope>& scope, std::shared_ptr<Object> object) {
    if(object==nullptr) {
        error(scope, "Expression does not exist");
    }
    return object;
}

std::shared_ptr<Object> Object::exists(const std::shared_ptr<Scope>& scope, std::shared_ptr<Object> object, const std::string& explanation) {
    if(object==nullptr) {
        error(scope, "Expression does not exist: "+explanation);
    }
    return object;
}

std::shared_ptr<Object> Object::pop(const std::shared_ptr<Scope>& scope, std::shared_ptr<Object> object) {
    auto ret = exists(scope, object);
    if(scope==nullptr)
        return ret;
    if(scope->owner==nullptr)
        return ret;
    scope->owner->stack.pop_back();
    return ret;
}


void Object::set(int i, std::shared_ptr<Object> value){
    error(std::shared_ptr<Scope>(nullptr), "Element setter not supported.");
};

std::shared_ptr<Object> Object::get(int i){
    error(std::shared_ptr<Scope>(nullptr), "Element getter not supported.");
    return shared_from_this();
};

int Object::size() {
    return 0;
};
