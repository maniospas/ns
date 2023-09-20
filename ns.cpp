#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <ctype.h>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <pthread.h>
#include "parser.h"
#include "Scope.h"
#include "CustomPredicateExecutor.h"
#include "Number.h"
#include "Thread.h"
#include "PString.h"
#include "Unscoped.h"


bool ends_with(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) 
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

void* async_worker(void* arg) {
    void** args = (void**)arg;
    {
        std::shared_ptr<Object> runnable = *((std::shared_ptr<Object>*)args[0]);
        std::shared_ptr<Scope> scope = *((std::shared_ptr<Scope>*)args[1]);
        std::shared_ptr<Thread> thread = *((std::shared_ptr<Thread>*)args[2]);

        scope->lock(thread);

        /*scope->thread_lock();
        scope->threads.push_back(thread); // do not remove this here, it will be removed on accessing
        scope->thread_unlock();*/

        auto ret = runnable->value(scope);
        if(ret==nullptr) 
            error(scope, "async nsbody evaluates to None");
        thread->result = ret;
        
        scope->unlock(thread);
    }
    delete (std::shared_ptr<Object>*)args[0];
    delete (std::shared_ptr<Scope>*)args[1];
    delete (std::shared_ptr<Thread>*)args[2];
    delete[] args;
    pthread_exit(NULL);
    return NULL;
}

class AsyncExecutor: public CustomPredicateExecutor {
    public:
        AsyncExecutor(): CustomPredicateExecutor("async((nsbody))") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto ret = exists(scope, scope->get("nsbody"), "nsbody");
            void** args = new void*[3];
            auto thread = std::make_shared<Thread>();
            args[0] = new std::shared_ptr<Object>(ret);
            args[1] = new std::shared_ptr<Scope>(scope);
            args[2] = new std::shared_ptr<Thread>(thread);
            pthread_create(thread->value(), NULL, async_worker, (void*)args);
            return thread;
        }
};


class JoinExecutor: public CustomPredicateExecutor {
    public:
        JoinExecutor(): CustomPredicateExecutor("join(nsthread)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto thread = std::dynamic_pointer_cast<Thread>(exists(scope, scope->get("nsthread"), "nsthread"));
            exists(scope, thread, "is not a thread (create threads with async(...))");
            pthread_join(*thread->value(), NULL);
            return thread->result;
        }
}; 

class DetachExecutor: public CustomPredicateExecutor {
    public:
        DetachExecutor(): CustomPredicateExecutor("pop(nspopfield)") {
        }
        std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope) {
            return CustomPredicateExecutor::predicateScope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto ret = exists(scope, scope->get("nspopfield"), "nspopfield");
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
        error(scope, "Error in MultiByteToWideChar");
    wchar_t* wideDllPath = new wchar_t[requiredSize];
    if (MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideDllPath, requiredSize) == 0) {
        delete[] wideDllPath;
        error(scope, "Error in MultiByteToWideChar");
    }
    HMODULE hDLL = LoadLibraryW(wideDllPath);
    if (hDLL == NULL) 
        error(scope, "Failed to load "+name);
    DLL_run run = (DLL_run)GetProcAddress(hDLL, "run");
    return run(scope);
}

bool file_exists(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

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
        error(scope, "Compilation failure");
    return run_dll(compiled, scope);
}


#else
#include <dlfcn.h>
typedef std::shared_ptr<Object> (*DLL_run)(std::shared_ptr<Scope>);

std::shared_ptr<Object> run_dll(std::string& name, std::shared_ptr<Scope> scope) {
    void* hDLL = dlopen(name, RTLD_LAZY);
    if (hDLL == NULL) 
        error(scope, "Failed to load "+name);
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
                std::shared_ptr<Object> obj = exists(scope, scope->get("nspath"), "nspath");
                if(std::dynamic_pointer_cast<String>(obj)!=nullptr) {
                    std::string source = obj->name();
                    if(ends_with(source, ".dll") || ends_with(source, ".so") || ends_with(source, ".dylib")) 
                        return run_dll(source, scope);
                    if(ends_with(source, ".cpp"))
                        return run_cpp(source, scope);
                    if(ends_with(source, ".ns"))
                        source = readfile(source);
                    obj = exists(scope, parse(source), "failed to parse file contents");
                }
                return exists(scope, obj->value(scope), "failed to parse string expression");
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
            std::list<std::shared_ptr<Object>> previous_stack(scope->owner->stack);
            scope->owner->stack.clear();
            try{
                auto ret = exists(scope, exists(scope, scope->get("nssource"), "nssource")->value(scope), "nssource value");
                scope->owner->stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
                scope->owner->stack = previous_stack;
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
            std::list<std::shared_ptr<Object>> previous_stack(scope->owner->stack);
            scope->owner->stack.clear();
            try{
                auto ret = exists(scope, exists(scope, scope->get("nssource"), "nssource")->value(scope), "nssource value");
                scope->owner->stack = previous_stack;
                return ret;
            }
            catch (std::runtime_error& e) {
                scope->owner->stack = previous_stack;
                auto except = exists(scope, exists(scope, scope->get("nsexcept"), "nsexcept")->value(scope), "nsexcept value");
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
                                            std::shared_ptr<Object>(nullptr),
                                            std::make_shared<Thread>());
    global->load(std::make_shared<DetachExecutor>());
    global->load(std::make_shared<LoadExecutor>());
    global->load(std::make_shared<TryExecutor>());
    global->load(std::make_shared<TryCatchExecutor>());
    global->load(std::make_shared<IsExecutor>());
    global->load(std::make_shared<AsyncExecutor>());
    global->load(std::make_shared<JoinExecutor>());

    auto program = parse(source);
    //std::cout << program->name() << std::endl;
    program->value(global);
    return 0;
}
