#include "String.h"


String::String(const std::string& value): value_(value) {
}

String::~String() {
}

const std::string String::name() const {
    return value_;
}

std::shared_ptr<Object> String::value(std::shared_ptr<Scope> scope) {
    return shared_from_this();
}
