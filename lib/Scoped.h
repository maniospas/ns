#ifndef SCOPED_H
#define SCOPED_H

#include "Object.h"


class Scoped: public Object
{
    private:
        std::shared_ptr<Object> object_;
    public:
        Scoped(std::shared_ptr<Object> object);
        virtual ~Scoped();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif // SCOPED_H
