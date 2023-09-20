#include "Sequence.h"
#include <memory>
#include <string>
#include "parser.h"


Sequence::Sequence(std::shared_ptr<Object> first, std::shared_ptr<Object> second) {
    if(std::dynamic_pointer_cast<Sequence>(first)) {
        auto seq = std::dynamic_pointer_cast<Sequence>(first);
        expressions.reserve(expressions.size() + seq->expressions.size());
        expressions.insert(expressions.end(), seq->expressions.begin(), seq->expressions.end());    
    }
    else
        expressions.push_back(first);
    if(std::dynamic_pointer_cast<Sequence>(second)) {
        auto seq = std::dynamic_pointer_cast<Sequence>(second);
        expressions.reserve(expressions.size() + seq->expressions.size());
        expressions.insert(expressions.end(), seq->expressions.begin(), seq->expressions.end());    
    }
    else
        expressions.push_back(second);
}

Sequence::~Sequence() {
}


const std::string Sequence::name() const {
    std::string ret = "";
    for(const auto& obj : expressions)
        if(obj!=nullptr) {
            if(ret.size())
                ret += ";";
            ret += obj->name();
        }
    return ret;
}

std::shared_ptr<Object> Sequence::value(std::shared_ptr<Scope> scope) {
    std::shared_ptr<Object> value = std::shared_ptr<Object>(nullptr);
    for(const auto& obj : expressions)
        if(obj!=nullptr)
            value = obj->value(scope);
    return value;
}


void Sequence::set(int i, std::shared_ptr<Object> value) {
    if(i<0 || i>=expressions.size())
        error(std::shared_ptr<Scope>(nullptr), "Setter index out of range");
    expressions[i] = value;
}
std::shared_ptr<Object> Sequence::get(int i) {
    if(i<0 || i>=expressions.size())
        error(std::shared_ptr<Scope>(nullptr), "Getter index out of range");
    return expressions[i];
}
int Sequence::size() {
    return expressions.size();
}