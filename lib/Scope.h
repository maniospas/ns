#ifndef SCOPE_H
#define SCOPE_H

#include <string>
#include <memory>
#include <map>
#include "Object.h"
class CustomPredicateExecutor;

class Scope: public Object {
    private:
        std::map<std::string, std::shared_ptr<Object>> values;
        std::shared_ptr<Object> overlap(const std::string& name, const std::shared_ptr<Object> value);
    public:
        Scope(const std::shared_ptr<Object> parent=std::shared_ptr<Object>(nullptr),
              const std::shared_ptr<Object> surface=std::shared_ptr<Object>(nullptr),
              const std::shared_ptr<Scope> prototype=std::shared_ptr<Scope>(nullptr),
              const std::shared_ptr<Scope> fallback=std::shared_ptr<Scope>(nullptr),
              const std::shared_ptr<Scope> failback=std::shared_ptr<Scope>(nullptr),
              const std::shared_ptr<Scope> fallfront=std::shared_ptr<Scope>(nullptr)
              );
        virtual ~Scope();
        void set(const std::string& name, const std::shared_ptr<Object> value);
        std::shared_ptr<Object> get(const std::string& name);
        std::shared_ptr<Scope> enter();
        int size() const;
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        void load(std::shared_ptr<CustomPredicateExecutor> method);
};

#endif
