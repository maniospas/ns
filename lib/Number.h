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
};

#endif
