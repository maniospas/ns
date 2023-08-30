#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
#include <list>
class Scope;


class Object: public std::enable_shared_from_this<Object> {
    protected:
        void push();
        std::shared_ptr<Object> exists(std::shared_ptr<Object> object);
        std::shared_ptr<Object> pop(std::shared_ptr<Object> object);
    public:
        Object();
        virtual const std::string name() const = 0;
        virtual std::shared_ptr<Object> value(std::shared_ptr<Scope> scope) = 0;
        virtual std::shared_ptr<Object> get(const std::string& name);
        virtual void set(const std::string& name, const std::shared_ptr<Object> value);
        virtual const std::string assignment_name() const;
        static std::string get_stack_trace();
        static std::list<std::shared_ptr<Object>> stack;

};

#endif // OBJECT_H
