#include "Object.h"
#include "Number.h"
#include "Scope.h"
#include "parser.h"
#include "CustomPredicateExecutor.h"
#include <string>
#include <memory>

class AddExecutor: public CustomPredicateExecutor {
    public:
        AddExecutor(): CustomPredicateExecutor("(a)+(b)") {
            priority = 4;
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a+b);
        }
};

class SubExecutor: public CustomPredicateExecutor {
    public:
        SubExecutor(): CustomPredicateExecutor("(a)-(b)") {
            priority = 3;
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a-b);
        }
};

class MulExecutor: public CustomPredicateExecutor {
    public:
        MulExecutor(): CustomPredicateExecutor("(a)*(b)") {
            priority = 2;
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a*b);
        }
};

class DivExecutor: public CustomPredicateExecutor {
    public:
        DivExecutor(): CustomPredicateExecutor("(a)/(b)") {
            priority = 1;
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            if(b==0)
                error("Division by zero");
            return std::make_shared<Number>(a/b);
        }
};


class LessExecutor: public CustomPredicateExecutor {
    public:
        LessExecutor(): CustomPredicateExecutor("(a)<(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a<b);
        }
};

class LessEqExecutor: public CustomPredicateExecutor {
    public:
        LessEqExecutor(): CustomPredicateExecutor("(a)<=(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a<=b);
        }
};

class GreaterExecutor: public CustomPredicateExecutor {
    public:
        GreaterExecutor(): CustomPredicateExecutor("(a)>(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a>b);
        }
};

class GreaterEqExecutor: public CustomPredicateExecutor {
    public:
        GreaterEqExecutor(): CustomPredicateExecutor("(a)>=(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(exists(scope->get("a")))->value();
            float b = std::dynamic_pointer_cast<Number>(exists(scope->get("b")))->value();
            return std::make_shared<Number>(a>=b);
        }
};



extern "C" __declspec(dllexport) std::shared_ptr<Object> run(std::shared_ptr<Scope> scope) {
    scope->load(std::make_shared<AddExecutor>());
    scope->load(std::make_shared<MulExecutor>());
    scope->load(std::make_shared<SubExecutor>());
    scope->load(std::make_shared<DivExecutor>());
    scope->load(std::make_shared<LessExecutor>());
    scope->load(std::make_shared<LessEqExecutor>());
    scope->load(std::make_shared<GreaterExecutor>());
    scope->load(std::make_shared<GreaterEqExecutor>());
    return scope;
};
