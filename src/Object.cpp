#include "Object.h"
#include "Variable.h"
#include "parser.h"


Object::Object() {
}


const std::string Object::assignment_name() const {
    return name();
}

std::shared_ptr<Object> Object::get(const std::string& name) {
    return std::shared_ptr<Object>(nullptr);
}

void Object::set(const std::string& name, const std::shared_ptr<Object> value) {
}

void Object::push() {
    stack.push_back(shared_from_this());
}

std::shared_ptr<Object> Object::exists(std::shared_ptr<Object> object) {
    if(object==nullptr)
        error("Predicate or variable does not exist");
    return object;
}

std::shared_ptr<Object> Object::pop(std::shared_ptr<Object> object) {
    auto ret = exists(object);
    stack.pop_back();
    return ret;
}


std::string Object::get_stack_trace() {
    std::string trace = "";
    for (auto obj : stack) {
        trace += "\nat "+obj->name();
    }
    return trace;
}

std::list<std::shared_ptr<Object>> Object::stack;


void Object::set(int i, std::shared_ptr<Object> value){
    error("Element setter not supported.");
};

std::shared_ptr<Object> Object::get(int i){
    error("Element getter not supported.");
    return shared_from_this();
};

int Object::size() {
    return 0;
};