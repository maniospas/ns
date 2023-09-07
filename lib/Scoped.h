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
        
        // vectorization
        void set(int i, std::shared_ptr<Object> value){object_->set(i, value);};
        std::shared_ptr<Object> get(int i){return object_->get(i);};
        int size(){return object_->size();};
        const std::string type() const {return "scope";};
};

#endif // SCOPED_H
