#ifndef UNSCOPED_H
#define UNSCOPED_H

#include "Object.h"


class Unscoped: public Object
{
    private:
        std::shared_ptr<Object> object_;
    public:
        Unscoped(std::shared_ptr<Object> object);
        virtual ~Unscoped();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        std::shared_ptr<Object> contents() const {return object_;};
};


#endif // UNSCOPED_H
