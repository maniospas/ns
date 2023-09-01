#ifndef PREDICATE_H
#define PREDICATE_H

#include <string>
#include <vector>
#include <memory>
#include "Object.h"
#include "PredicatePart.h"

class Predicate: public Object {
    public:
        std::vector<std::shared_ptr<PredicatePart>> names_;
        Predicate(std::vector<std::shared_ptr<PredicatePart>> names);
        virtual ~Predicate();
        const std::string name() const;
        const std::string assignment_name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif // PREDICATE_H
