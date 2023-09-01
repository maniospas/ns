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
#include "String.h"


bool ends_with(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) 
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}


class PrintExecutor: public CustomPredicateExecutor {
    public:
        PrintExecutor(): CustomPredicateExecutor("print(text)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto text = exists(scope->get("text"));
            std::cout << text->name() << std::endl;
            return text;
        }
};

class InputExecutor: public CustomPredicateExecutor {
    public:
        InputExecutor(): CustomPredicateExecutor("read(prompt)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::cout << exists(scope->get("prompt"))->name();
            std::string input;
            std::getline(std::cin, input);
            return std::make_shared<String>(input);
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
            auto ret = exists(scope->get("field"));
            scope->set(ret->name(), std::shared_ptr<Object>(nullptr));
            return ret;
        }
};


class AddExecutor: public CustomPredicateExecutor {
    public:
        AddExecutor(): CustomPredicateExecutor("(a)+(b)") {
            priority = 1;
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
            priority = 2;
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
            priority = 3;
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
            priority = 4;
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

class IfExecutor: public CustomPredicateExecutor {
    public:
        IfExecutor(): CustomPredicateExecutor("if(condition)(success)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
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
        std::shared_ptr<Object> evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope){
            return expression;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> ret = std::shared_ptr<Object>(nullptr);
            auto condition = scope->get("condition");
            auto loop = scope->get("loop");
            while(std::dynamic_pointer_cast<Number>(condition->value(scope))->value()) 
                ret = loop->value(scope);
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
        std::shared_ptr<Scope> descoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            //try{
                std::string source = exists(scope->get("path"))->name();
                if(ends_with(source, ".ns"))
                    source = readfile(source);
                return exists(exists(parse(source))->value(scope));
            /*}
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                return std::make_shared<String>(e.what());
            }*/
        }
};


class CatchExecutor: public CustomPredicateExecutor {
    public:
        CatchExecutor(): CustomPredicateExecutor("try{source}") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> evaluate_argument(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope){
            return expression;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::list<std::shared_ptr<Object>> previous_stack(Object::stack);
            Object::stack.clear();
            try{
                auto ret = exists(exists(scope->get("source"))->value(scope));
                Object::stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                Object::stack = previous_stack;
                return std::make_shared<String>(e.what());
            }
        }
};



int main(int argc, char *argv[]) {
    std::string source = "cli.ns";
    if(argc>1)
        source = argv[1];
    source = "run('"+source+"')";
    auto global = std::make_shared<Scope>();
    global->load(std::make_shared<PrintExecutor>());
    global->load(std::make_shared<InputExecutor>());
    global->load(std::make_shared<DetachExecutor>());
    global->load(std::make_shared<AddExecutor>());
    global->load(std::make_shared<MulExecutor>());
    global->load(std::make_shared<SubExecutor>());
    global->load(std::make_shared<DivExecutor>());
    global->load(std::make_shared<IfExecutor>());
    global->load(std::make_shared<IfElseExecutor>());
    global->load(std::make_shared<WhileExecutor>());
    global->load(std::make_shared<LessExecutor>());
    global->load(std::make_shared<LessEqExecutor>());
    global->load(std::make_shared<GreaterExecutor>());
    global->load(std::make_shared<GreaterEqExecutor>());
    global->load(std::make_shared<LoadExecutor>());
    global->load(std::make_shared<CatchExecutor>());
    auto program = parse(source);
    //std::cout << program->name() << std::endl;
    program->value(global);
    return 0;
}
