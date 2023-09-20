#include "Object.h"
#include "Scope.h"
#include "parser.h"
#include "CustomPredicateExecutor.h"
#include <string>
#include <memory>
#include "Number.h"



class IfExecutor: public CustomPredicateExecutor {
    public:
        IfExecutor(): CustomPredicateExecutor("if((nscondition))((nssuccess))") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("nscondition");
            std::shared_ptr<Object> block = scope->get("nssuccess");
            auto number = std::dynamic_pointer_cast<Number>(exists(scope, condition->value(scope), "nscondition value"));
            if(number!=nullptr && number->value())
                return block->value(scope);
            return std::shared_ptr<Object>(nullptr);
        }
};


class IfElseExecutor: public CustomPredicateExecutor {
    public:
        IfElseExecutor(): CustomPredicateExecutor("if((nscondition))((nssuccess))else((nsfailure))") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("nscondition");
            std::shared_ptr<Object> block = scope->get("nssuccess");
            std::shared_ptr<Object> alternative = scope->get("nsfailure");
            auto number = std::dynamic_pointer_cast<Number>(exists(scope, condition->value(scope), "nscondition value"));
            if(number!=nullptr && number->value())
                return block->value(scope);
            return alternative->value(scope);
        }
};


class WhileExecutor: public CustomPredicateExecutor {
    public:
        WhileExecutor(): CustomPredicateExecutor("while((nscondition))((nsloop))") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> ret = std::shared_ptr<Object>(nullptr);
            auto condition = scope->get("nscondition");
            auto loop = scope->get("nsloop");
            while(true) {
                auto number = std::dynamic_pointer_cast<Number>(condition->value(scope));
                if(number==nullptr || !number->value())
                    break;
                ret = loop->value(scope);
            }
            return ret;
        }
};

extern "C" __declspec(dllexport) std::shared_ptr<Object> run(std::shared_ptr<Scope> scope) {
    scope->load(std::make_shared<IfExecutor>());
    scope->load(std::make_shared<IfElseExecutor>());
    scope->load(std::make_shared<WhileExecutor>());
    return scope;
};

