#ifndef ASSIGN_H
#define ASSIGN_H

#include <string>
#include "Object.h"

class Assign: public Object {
    private:
        std::shared_ptr<Object> name_;
        std::shared_ptr<Object> object_;
    public:
        Assign(std::shared_ptr<Object> name, std::shared_ptr<Object> object);
        virtual ~Assign();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif
