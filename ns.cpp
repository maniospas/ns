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
#include "PString.h"
#include "Unscoped.h"


bool ends_with(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) 
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

class DetachExecutor: public CustomPredicateExecutor {
    public:
        DetachExecutor(): CustomPredicateExecutor("pop(nspopfield)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto ret = exists(scope->get("nspopfield"), "nspopfield");
            scope->set(ret->name(), std::shared_ptr<Object>(nullptr));
            return ret;
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


/*#include <restbed.hpp>


void helloWorldHandler(const std::shared_ptr<restbed::Session> session) {
    const auto request = session->get_request();
    std::string response_body = "Hello, World!";
    session->close(restbed::OK, response_body, {{"Content-Length", std::to_string(response_body.length())}});
}*/


std::string readfile(std::string path) {
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}

/*std::shared_ptr<Object> run_dll(std::string& name, std::shared_ptr<Scope> scope){
    return scope;
}
*/


#ifdef _WIN32
#include <Windows.h>
typedef std::shared_ptr<Object> (*DLL_run)(std::shared_ptr<Scope>);

std::shared_ptr<Object> run_dll(std::string& name, std::shared_ptr<Scope> scope) {
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
    if (requiredSize == 0) 
        error("Error in MultiByteToWideChar");
    wchar_t* wideDllPath = new wchar_t[requiredSize];
    if (MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideDllPath, requiredSize) == 0) {
        delete[] wideDllPath;
        error("Error in MultiByteToWideChar");
    }
    HMODULE hDLL = LoadLibraryW(wideDllPath);
    if (hDLL == NULL) 
        error("Failed to load "+name);
    DLL_run run = (DLL_run)GetProcAddress(hDLL, "run");
    return run(scope);
}

bool file_exists(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

#include<filesystem>

std::shared_ptr<Object> run_cpp(std::string& name_, std::shared_ptr<Scope> scope) {
    std::string name = std::filesystem::current_path().string()+"/"+name_;
    std::string compiled = name.substr(0, name.length()-4)+".dll";
    if(!file_exists(compiled.c_str())) {
        std::cout << "Please wait while "+compiled << " is compiled for the first time\n";
        auto compiler_object = scope->get("nscompiler");
        std::string command = compiler_object==nullptr?"g++":compiler_object->name();
        command += " -shared -g \""+name+"\" -o \""+compiled+"\" src/*.cpp -Ilib -O3";
        //std::cout << command<<"\n";
        std::system(command.c_str());
    }
    if(!file_exists(compiled.c_str())) 
        error("Compilation failure");
    return run_dll(compiled, scope);
}


#else
#include <dlfcn.h>
typedef std::shared_ptr<Object> (*DLL_run)(std::shared_ptr<Scope>);

std::shared_ptr<Object> run_dll(std::string& name, std::shared_ptr<Scope> scope) {
    void* hDLL = dlopen(name, RTLD_LAZY);
    if (hDLL == NULL) 
        error("Failed to load "+name);
    DLL_run run = (DLL_run)dlsym(libraryHandle, "run");
    return run(scope);
}
#endif

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
                std::shared_ptr<Object> obj = exists(scope->get("nspath"), "nspath");
                if(std::dynamic_pointer_cast<String>(obj)!=nullptr) {
                    std::string source = obj->name();
                    if(ends_with(source, ".dll") || ends_with(source, ".so") || ends_with(source, ".dylib")) 
                        return run_dll(source, scope);
                    if(ends_with(source, ".cpp"))
                        return run_cpp(source, scope);
                    if(ends_with(source, ".ns"))
                        source = readfile(source);
                    obj = exists(parse(source), "failed to parse file contents");
                }
                return exists(obj->value(scope), "failed to parse string expression");
            /*}
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                return std::make_shared<String>(e.what());
            }*/
        }
};


class TryExecutor: public CustomPredicateExecutor {
    public:
        TryExecutor(): CustomPredicateExecutor("try((nssource))") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::list<std::shared_ptr<Object>> previous_stack(Object::stack);
            Object::stack.clear();
            try{
                auto ret = exists(exists(scope->get("nssource"), "nssource")->value(scope));
                Object::stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                Object::stack = previous_stack;
                return std::make_shared<String>(e.what());
                //return std::make_shared<Number>(0);
            }
        }
};


class TryCatchExecutor: public CustomPredicateExecutor {
    public:
        TryCatchExecutor(): CustomPredicateExecutor("try((nssource))((nsexcept))") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::list<std::shared_ptr<Object>> previous_stack(Object::stack);
            Object::stack.clear();
            try{
                auto ret = exists(exists(scope->get("nssource"), "nssource")->value(scope), "nssource value");
                Object::stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                Object::stack = previous_stack;
                auto except = exists(exists(scope->get("nsexcept"), "nsexcept")->value(scope), "nsexcept value");
                return except;
            }
        }
};



int main(int argc, char *argv[]) {
    std::string source = "cli.ns";
    if(argc>1)
        source = argv[1];
    source = "run('"+source+"')";
    auto global = std::make_shared<Scope>(std::shared_ptr<Object>(nullptr), 
                                            std::shared_ptr<Object>(nullptr), 
                                            std::shared_ptr<Object>(nullptr));
    global->load(std::make_shared<DetachExecutor>());
    global->load(std::make_shared<LoadExecutor>());
    global->load(std::make_shared<TryExecutor>());
    global->load(std::make_shared<TryCatchExecutor>());
    global->load(std::make_shared<IsExecutor>());

    auto program = parse(source);
    //std::cout << program->name() << std::endl;
    program->value(global);
    return 0;
}
