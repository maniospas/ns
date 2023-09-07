#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include <memory>
#include "Object.h"
#include "PredicatePart.h"

class Expression: public Object {
    public:
        std::vector<std::shared_ptr<PredicatePart>> names_;
        Expression(std::vector<std::shared_ptr<PredicatePart>> names);
        virtual ~Expression();
        const std::string name() const;
        const std::string assignment_name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        const std::string type() const {return "expression";};
};

#endif // EXPRESSION_H
