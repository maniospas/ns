#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <Object.h>


class Variable: public Object {
    private:
        const std::string name_;
    public:
        Variable(const std::string& name);
        virtual ~Variable();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        const std::string type() const {return "variable";};
};

#endif
