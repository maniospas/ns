#ifndef EDIT_H
#define EDIT_H

#include <string>
#include "Object.h"

class Edit: public Object {
    private:
        std::shared_ptr<Object> object_;
        std::shared_ptr<Object> name_;
    public:
        Edit(std::shared_ptr<Object> object, std::shared_ptr<Object> name);
        virtual ~Edit();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        std::shared_ptr<Object>& object() {return object_;};
        std::shared_ptr<Object>& expression() {return name_;};
};

#endif
