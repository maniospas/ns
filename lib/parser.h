#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <vector>
#include <string>
#include <memory>
#include "Object.h"
#include "PredicatePart.h"
#include "CustomPredicateExecutor.h"
#include "Scope.h"



class Callable: public CustomPredicateExecutor {
    private:
        std::shared_ptr<Object> call_;
    public:
        Callable(std::string signature, std::string call);
        Callable(std::string signature, std::shared_ptr<Object> call);
        Callable(std::vector<std::shared_ptr<PredicatePart>> signature, std::shared_ptr<Object> call);
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope);
        std::shared_ptr<Scope> descoped(std::shared_ptr<Scope> scope);
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope);
        const std::string name() const;
        const std::string assignment_name() const;
};

void error(const std::string& message);
std::vector<std::string> tokenize(const std::string& source);
std::shared_ptr<Object> parse(std::vector<std::string>& tokens, int from, int to);
std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string> tokens);
std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string>& tokens, int from, int to);
std::shared_ptr<Object> parse(const std::string& source);
bool is_number_convertible(std::string predicate);

#endif
