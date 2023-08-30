#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
class Scope;


class Object: public std::enable_shared_from_this<Object> {
    public:
        Object();
        virtual const std::string name() const = 0;
        virtual std::shared_ptr<Object> value(std::shared_ptr<Scope> scope) = 0;
        virtual std::shared_ptr<Object> get(const std::string& name);
        virtual void set(const std::string& name, const std::shared_ptr<Object> value);
        virtual const std::string assignment_name() const;
};

#endif // OBJECT_H
