#include "Object.h"
#include "Variable.h"

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
