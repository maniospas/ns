#include "Object.h"
#include "Scope.h"
#include "parser.h"
#include "CustomPredicateExecutor.h"
#include <string>
#include <memory>
#include <iostream>
#include "PString.h"


class PrintExecutor: public CustomPredicateExecutor {
    public:
        PrintExecutor(): CustomPredicateExecutor("print(text)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            auto text = exists(scope->get("text"), "text");
            std::cout << text->name() << std::endl;
            return text;
        }
};

class InputExecutor: public CustomPredicateExecutor {
    public:
        InputExecutor(): CustomPredicateExecutor("read(prompt)") {
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            std::cout << exists(scope->get("prompt"), "prompt")->name();
            std::string input;
            std::getline(std::cin, input);
            return std::make_shared<String>(input);
        }
};

extern "C" __declspec(dllexport) std::shared_ptr<Object> run(std::shared_ptr<Scope> scope) {
    scope->load(std::make_shared<PrintExecutor>());
    scope->load(std::make_shared<InputExecutor>());
    return scope;
};
