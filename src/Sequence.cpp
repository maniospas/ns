#include "Sequence.h"

Sequence::Sequence(std::shared_ptr<Object> first, std::shared_ptr<Object> second): first_(first), second_(second) {
}

Sequence::~Sequence() {
}


const std::string Sequence::name() const {
    return first_->name()+";"+second_->name();
}

std::shared_ptr<Object> Sequence::value(std::shared_ptr<Scope> scope) {
    first_->value(scope);
    return second_->value(scope);
}

