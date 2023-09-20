#include "Object.h"
#include "Scope.h"
#include "parser.h"
#include "CustomPredicateExecutor.h"
#include <string>
#include <memory>
#include "PString.h"
#include "Number.h"
#include "Unscoped.h"


class SizeExecutor: public CustomPredicateExecutor {
    public:
        SizeExecutor(): CustomPredicateExecutor("size(nsblock)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope, scope->get("nsblock"), "nsblock");
            return std::make_shared<Number>(block->size());
        }
};

class GetExecutor: public CustomPredicateExecutor {
    public:
        GetExecutor(): CustomPredicateExecutor("get(nsblock)(nsindex)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope, scope->get("nsblock"), "nsblock");
            std::shared_ptr<Object> i_obj = exists(scope, scope->get("nsindex"), "nsindex");
            auto i = std::dynamic_pointer_cast<Number>(i_obj);
            if(i!=nullptr)
                return block->get((int)i->value());
            if(std::dynamic_pointer_cast<Unscoped>(i_obj)!=nullptr)
                i_obj = std::dynamic_pointer_cast<Unscoped>(i_obj)->contents();
            return block->get(i_obj->name());
            //exists(scope, i, "nsindex is not a number (use nsblock:get(nsindex) instead)");
        }
};

class SetExecutor: public CustomPredicateExecutor {
    public:
        SetExecutor(): CustomPredicateExecutor("set(nsblock)(nsindex)(nsvalue)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope, scope->get("nsblock"), "nsblock");
            std::shared_ptr<Object> i_obj = exists(scope, scope->get("nsindex"), "nsindex");
            std::shared_ptr<Object> value = scope->get("nsvalue");
            auto i = std::dynamic_pointer_cast<Number>(i_obj);
            exists(scope, i, "nsindex not a number");
            block->set((int)i->value(), value);
            return value;
        }
};


class SelfSetExecutor: public CustomPredicateExecutor {
    public:
        SelfSetExecutor(): CustomPredicateExecutor("set(nsname)(nsvalue)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> name = exists(scope, scope->get("nsname"), "nsname");
            std::shared_ptr<Object> value = scope->get("nsvalue");
            scope->set(name->name(), value);
            return value;
        }
};


class SelfGetExecutor: public CustomPredicateExecutor {
    public:
        SelfGetExecutor(): CustomPredicateExecutor("get(nsname)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> name = exists(scope, scope->get("nsname"), "nsname");
            return scope->get(name->name());
        }
};


extern "C" __declspec(dllexport) std::shared_ptr<Object> run(std::shared_ptr<Scope> scope) {
    scope->load(std::make_shared<SizeExecutor>());
    scope->load(std::make_shared<GetExecutor>());
    scope->load(std::make_shared<SetExecutor>());
    scope->load(std::make_shared<SelfSetExecutor>());
    scope->load(std::make_shared<SelfGetExecutor>());
    return scope;
};