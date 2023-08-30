#include "Number.h"

Number::Number(const float value): value_(value) {
}

Number::~Number() {
}

const std::string Number::name() const {
    return std::to_string(value_);
}

std::shared_ptr<Object> Number::value(std::shared_ptr<Scope> scope) {
    return shared_from_this();
}

const float Number::value() const {
    return value_;
}
