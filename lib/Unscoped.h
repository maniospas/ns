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
        
        // vectorization
        void set(int i, std::shared_ptr<Object> value){object_->set(i, value);};
        std::shared_ptr<Object> get(int i){return object_->get(i);};
        int size(){return object_->size();};
};


#endif // UNSCOPED_H
