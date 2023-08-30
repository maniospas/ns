#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <ctype.h>
#include <fstream>
#include <streambuf>
#include "parser.h"
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "Number.h"


class PrintExecutor: public CustomPredicateExecutor {
    public:
        PrintExecutor(): CustomPredicateExecutor("print(text)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::cout << scope->get("text")->name() << std::endl;
            return scope->get("text");
        }
};

class DetachExecutor: public CustomPredicateExecutor {
    public:
        DetachExecutor(): CustomPredicateExecutor("pop(field)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> derived_scope) {
            auto scope = std::make_shared<Scope>(derived_scope, derived_scope->get("super"));
            auto ret = scope->get("field");
            scope->set(ret->name(), std::shared_ptr<Object>(nullptr));
            return ret;
        }
};


class AddExecutor: public CustomPredicateExecutor {
    public:
        AddExecutor(): CustomPredicateExecutor("(a)+(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a+b);
        }
};

class SubExecutor: public CustomPredicateExecutor {
    public:
        SubExecutor(): CustomPredicateExecutor("(a)-(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a-b);
        }
};

class MulExecutor: public CustomPredicateExecutor {
    public:
        MulExecutor(): CustomPredicateExecutor("(a)*(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a*b);
        }
};


class LessExecutor: public CustomPredicateExecutor {
    public:
        LessExecutor(): CustomPredicateExecutor("(a)<(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a<b);
        }
};

class LessEqExecutor: public CustomPredicateExecutor {
    public:
        LessEqExecutor(): CustomPredicateExecutor("(a)<=(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a<=b);
        }
};

class GreaterExecutor: public CustomPredicateExecutor {
    public:
        GreaterExecutor(): CustomPredicateExecutor("(a)>(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a>b);
        }
};

class GreaterEqExecutor: public CustomPredicateExecutor {
    public:
        GreaterEqExecutor(): CustomPredicateExecutor("(a)>=(b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            float a = std::dynamic_pointer_cast<Number>(scope->get("a"))->value();
            float b = std::dynamic_pointer_cast<Number>(scope->get("b"))->value();
            return std::make_shared<Number>(a>=b);
        }
};

class IfExecutor: public CustomPredicateExecutor {
    public:
        IfExecutor(): CustomPredicateExecutor("if(condition)(success)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope){
            return expression;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("condition");
            std::shared_ptr<Object> block = scope->get("success");
            if(std::dynamic_pointer_cast<Number>(condition->value(scope))->value())
                return block->value(scope);
            return std::shared_ptr<Object>(nullptr);
        }
};


class IfElseExecutor: public CustomPredicateExecutor {
    public:
        IfElseExecutor(): CustomPredicateExecutor("if(condition)(success)else(failure)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope){
            return expression;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("condition");
            std::shared_ptr<Object> block = scope->get("success");
            std::shared_ptr<Object> alternative = scope->get("failure");
            if(std::dynamic_pointer_cast<Number>(condition->value(scope))->value())
                return block->value(scope);
            return alternative->value(scope);
        }
};


class WhileExecutor: public CustomPredicateExecutor {
    public:
        WhileExecutor(): CustomPredicateExecutor("while(condition)(loop)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope){
            return expression;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("condition");
            std::shared_ptr<Object> block = scope->get("loop");
            std::shared_ptr<Object> ret = std::shared_ptr<Object>(nullptr);
            while(std::dynamic_pointer_cast<Number>(condition->value(scope))->value())
                ret = block->value(scope);
            return ret;
        }
};


std::string readfile(std::string path) {
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}


class LoadExecutor: public CustomPredicateExecutor {
    public:
        LoadExecutor(): CustomPredicateExecutor("run{path}") {
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::string source = readfile(scope->get("path")->name());
            return parse(source)->value(scope);
        }
};


int main(int argc, char *argv[]) {
    /*if(argc<2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  newsc <file.ns>" << std::endl;
        return 0;
    }
    std::string file = argv[1];*/
    std::string file = "test.ns";
    if(argc>1)
        file = argv[1];
    std::string source = readfile(file);
    auto global = std::make_shared<Scope>();
    global->load(std::make_shared<PrintExecutor>());
    global->load(std::make_shared<DetachExecutor>());
    global->load(std::make_shared<AddExecutor>());
    global->load(std::make_shared<MulExecutor>());
    global->load(std::make_shared<SubExecutor>());
    global->load(std::make_shared<IfExecutor>());
    global->load(std::make_shared<IfElseExecutor>());
    global->load(std::make_shared<WhileExecutor>());
    global->load(std::make_shared<LessExecutor>());
    global->load(std::make_shared<LessEqExecutor>());
    global->load(std::make_shared<GreaterExecutor>());
    global->load(std::make_shared<GreaterEqExecutor>());
    global->load(std::make_shared<LoadExecutor>());
    auto program = parse(source);
    //std::cout << program->name() << std::endl;
    program->value(global);
    return 0;
}
