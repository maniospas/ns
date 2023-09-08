#ifndef NUMBER_H
#define NUMBER_H

#include <string>
#include "Object.h"

class Number: public Object
{
    private:
        float value_;
    public:
        Number(const float value);
        virtual ~Number();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        const float value() const;
        const std::string type() const {return "number";};
        bool is_primitive() {return true;};
        bool is_parsed() {return true;};
};

#endif
