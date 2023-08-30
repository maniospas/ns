#ifndef ACCESS_H
#define ACCESS_H

#include <string>
#include "Object.h"

class Access: public Object {
    private:
        std::shared_ptr<Object> object_;
        std::shared_ptr<Object> name_;
    public:
        Access(std::shared_ptr<Object> object, std::shared_ptr<Object> name);
        virtual ~Access();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif // ACCESS_H
