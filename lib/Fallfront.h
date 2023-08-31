#ifndef FALLFRONT_H
#define FALLFRONT_H

#include <string>
#include "Object.h"

class Fallfront: public Object {
    private:
        std::shared_ptr<Object> object_;
        std::shared_ptr<Object> name_;
    public:
        Fallfront(std::shared_ptr<Object> object, std::shared_ptr<Object> name);
        virtual ~Fallfront();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif
