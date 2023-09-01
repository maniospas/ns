#include "PredicatePart.h"
#include "parser.h"


PredicatePart::PredicatePart() {
}

PredicatePart::~PredicatePart() {
}

PredicatePartWord::PredicatePartWord(const std::string& name): name_(name) {
}

std::string PredicatePartWord::name() const {
    return name_;
}

std::string PredicatePartWord::assignment_name() const {
    return name_;
}


std::shared_ptr<Object> PredicatePartWord::object() const {
    return std::shared_ptr<Object>(nullptr);
}


PredicatePartObject::PredicatePartObject(std::shared_ptr<Object> object): object_(object) {
}

std::string PredicatePartObject::assignment_name() const {
    return "()";
}

std::string PredicatePartObject::name() const {
    return object_->name();
}

std::shared_ptr<Object> PredicatePartObject::object() const {
    return object_;
}

