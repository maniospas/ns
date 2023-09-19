#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
#include <list>
class Scope;


class Object: public std::enable_shared_from_this<Object> {
    protected:
        void push();
        std::shared_ptr<Object> pop(std::shared_ptr<Object> object);
        std::shared_ptr<Object> exists(std::shared_ptr<Object> object);
        std::shared_ptr<Object> exists(std::shared_ptr<Object> object, const std::string& explanation);
    public:
        Object();
        virtual const std::string type() const = 0;
        virtual const std::string name() const = 0;
        virtual void lock(){};
        virtual void unlock(){};
        virtual std::shared_ptr<Object> value(std::shared_ptr<Scope> scope) = 0;
        virtual std::shared_ptr<Object> get(const std::string& name);
        virtual void set(const std::string& name, const std::shared_ptr<Object> value);
        virtual const std::string assignment_name() const;
        static std::string get_stack_trace();
        static void exit_stack(int from);
        static std::list<std::shared_ptr<Object>> stack;
        virtual bool is_primitive() {return false;};
        virtual bool is_parsed() {return false;};
        std::shared_ptr<Object> safe_get(const std::string& name) {
            return exists(get(name), name);
        };

        // vectorization
        virtual void set(int i, std::shared_ptr<Object> value);
        virtual std::shared_ptr<Object> get(int i);
        virtual int size();
};

#endif // OBJECT_H
