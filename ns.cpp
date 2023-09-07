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

class IsExecutor: public CustomPredicateExecutor {
    public:
        IsExecutor(): CustomPredicateExecutor("(a) is (b)") {
        }
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto a = scope->get("a");
            auto b = scope->get("b");
            if(a==b)
                return std::make_shared<Number>(1);
            if(a==nullptr || b==nullptr) 
                return std::make_shared<Number>(0);
            if(a->assignment_name()==b->assignment_name())
                return std::make_shared<Number>(1);
            return std::make_shared<Number>(0);
        }
};

class IfExecutor: public CustomPredicateExecutor {
    public:
        IfExecutor(): CustomPredicateExecutor("if((nscondition))((nssuccess))") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> condition = scope->get("nscondition");
            std::shared_ptr<Object> block = scope->get("nssuccess");
            auto number = std::dynamic_pointer_cast<Number>(exists(condition->value(scope)));
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
            auto number = std::dynamic_pointer_cast<Number>(exists(condition->value(scope)));
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



class SizeExecutor: public CustomPredicateExecutor {
    public:
        SizeExecutor(): CustomPredicateExecutor("size(nsblock)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope->get("nsblock"));
            return std::make_shared<Number>(block->size());
        }
};

class GetExecutor: public CustomPredicateExecutor {
    public:
        GetExecutor(): CustomPredicateExecutor("get(nsblock)(nsindex)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope->get("nsblock"));
            std::shared_ptr<Object> i_obj = exists(scope->get("nsindex"));
            auto i = std::dynamic_pointer_cast<Number>(i_obj);
            exists(i);
            return block->get((int)i->value());
        }
};

class SetExecutor: public CustomPredicateExecutor {
    public:
        SetExecutor(): CustomPredicateExecutor("set(nsblock)(nsindex)(nsvalue)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> block = exists(scope->get("nsblock"));
            std::shared_ptr<Object> i_obj = exists(scope->get("nsindex"));
            std::shared_ptr<Object> value = scope->get("nsvalue");
            auto i = std::dynamic_pointer_cast<Number>(i_obj);
            exists(i);
            block->set((int)i->value(), value);
            return value;
        }
};


class SelfSetExecutor: public CustomPredicateExecutor {
    public:
        SelfSetExecutor(): CustomPredicateExecutor("set(nsname)(nsvalue)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::shared_ptr<Object> name = exists(scope->get("nsname"));
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
            std::shared_ptr<Object> name = exists(scope->get("nsname"));
            return scope->get(name->name());
        }
};




std::string readfile(std::string path) {
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}


class LoadExecutor: public CustomPredicateExecutor {
    public:
        LoadExecutor(): CustomPredicateExecutor("run(nspath)") {
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Scope> descoped(std::shared_ptr<Scope> scope) {
            return scope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            //try{
                std::shared_ptr<Object> obj = exists(scope->get("nspath"));
                if(std::dynamic_pointer_cast<String>(obj)!=nullptr) {
                    std::string source = obj->name();
                    if(ends_with(source, ".ns"))
                        source = readfile(source);
                    obj = exists(parse(source));
                }
                return exists(obj->value(scope));
            /*}
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                return std::make_shared<String>(e.what());
            }*/
        }
};


class CatchExecutor: public CustomPredicateExecutor {
    public:
        CatchExecutor(): CustomPredicateExecutor("try((nssource))") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::list<std::shared_ptr<Object>> previous_stack(Object::stack);
            Object::stack.clear();
            try{
                auto ret = exists(exists(scope->get("nssource"))->value(scope));
                Object::stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                //Object::stack = previous_stack;
                return std::make_shared<String>(e.what());
                //return std::make_shared<Number>(0);
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
    global->load(std::make_shared<SizeExecutor>());
    global->load(std::make_shared<GetExecutor>());
    global->load(std::make_shared<SetExecutor>());
    global->load(std::make_shared<SelfSetExecutor>());
    global->load(std::make_shared<SelfGetExecutor>());
    global->load(std::make_shared<IsExecutor>());
    auto program = parse(source);
    //std::cout << program->name() << std::endl;
    program->value(global);
    return 0;
}
