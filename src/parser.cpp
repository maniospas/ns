#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "parser.h"
#include "Variable.h"
#include "Scope.h"
#include "Number.h"
#include "Sequence.h"
#include "Assign.h"
#include "Scoped.h"
#include "Unscoped.h"
#include "Access.h"
#include "Edit.h"
#include "Fallfront.h"
#include "String.h"
#include "Predicate.h"
#include "PredicatePart.h"
#include "CustomPredicateExecutor.h"


bool is_number_convertible(std::string predicate) {
    int digits = predicate.length();
    for(int i=0;i<digits;i++)
        if(!isdigit(predicate[i]) && predicate[i]!='.')
            return false;
    return digits;
}


class Callable: public CustomPredicateExecutor {
    private:
        std::shared_ptr<Object> call_;
    public:
        Callable(std::string signature, std::string call): CustomPredicateExecutor(predicate_parts(tokenize(signature))), call_(parse(call)) {
        }
        Callable(std::string signature, std::shared_ptr<Object> call): CustomPredicateExecutor(predicate_parts(tokenize(signature))), call_(call) {
        }
        Callable(std::vector<std::shared_ptr<PredicatePart>> signature, std::shared_ptr<Object> call): CustomPredicateExecutor(signature), call_(call) {
        }
        std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope) {
            //if(std::dynamic_pointer_cast<Scoped>(call_) != nullptr)
            //    return scope->enter();
            return scope->enter();
        }
        std::shared_ptr<Scope> descoped(std::shared_ptr<Scope> scope) {
            if(std::dynamic_pointer_cast<Scoped>(call_) == nullptr)
                scope->set("new", scope->get("super"));
            return scope;
        }
        std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) {
            return call_->value(scope);
        }
        const std::string name() const {
            return CustomPredicateExecutor::name()+":="+call_->name();
        }
        const std::string assignment_name() const {
            return CustomPredicateExecutor::assignment_name();
        }
};

void error(const std::string& message) {
    throw std::runtime_error(message+Object::get_stack_trace());
}

std::vector<std::string> tokenize(const std::string& source) {
    std::vector<std::string> tokens;
    std::string pending = "";
    char last_parenthesis = ',';
    char last_parenthesis_close = ' ';
    int sN = source.length();
    bool comments = false;
    bool string_mode = false;
    for(int i=0;i<sN;i++){
        char c = source[i];
        if(c=='"')
            c = '\'';
        if(c=='\'')
            string_mode = !string_mode;
        if(c=='/' && i<sN-1 && source[i+1]=='/' && !string_mode)
            comments = true;
        if(c=='\n' && !string_mode) {
            c = ' ';
            comments = false;
        }
        if(comments)
            continue;
        if(c=='\t')
            c = ' ';
        if(!isalpha(c) && !isdigit(c)) {
            if(pending.length())
                tokens.push_back(pending);
            if(c==',' && last_parenthesis!=',' && !string_mode) {
                tokens.push_back(std::string(1, last_parenthesis_close));
                tokens.push_back(std::string(1, last_parenthesis));
            }
            else
                tokens.push_back(std::string(1, c));
            pending = "";
        }
        else
            pending += c;
        if(c=='('){
            last_parenthesis = '(';
            last_parenthesis_close = ')';
        }
        if(c=='{'){
            last_parenthesis = '{';
            last_parenthesis_close = '}';
        }
        if(c=='['){
            last_parenthesis = '[';
            last_parenthesis_close = ']';
        }
    }
    if(pending.length())
        tokens.push_back(pending);
    std::vector<std::string> merged_tokens;
    int i = 0;
    int N = tokens.size();
    while(true) {
        std::string token = tokens[i];
        if(i==N-1) {
            merged_tokens.push_back(token);
            break;
        }
        std::string next_token = tokens[i+1];
        if(is_number_convertible(token) && next_token==".") {
            if(i<N-1 && is_number_convertible(tokens[i+2])) {
                token += "."+tokens[i+2];
                i++;
            }
            merged_tokens.push_back(token);
            i += 2;
        }
        else if(token.length()==1 && next_token.length()==1 && token!=" " && !isalpha(token[0]) && !isdigit(token[1]) && next_token=="=") {
            merged_tokens.push_back(token+next_token);
            i += 2;
        }
        /*else if(token=="-" && next_token==">") {
            merged_tokens.push_back(".");
            i += 2;
        }
        else if(token=="-" && next_token=="<") {
            merged_tokens.push_back(":");
            i += 2;
        }*/
        else {
            merged_tokens.push_back(token);
            i++;
        }
    }
    return merged_tokens;
}

std::shared_ptr<Object> parse(std::vector<std::string>& tokens, int from, int to) {
    if(from>to)
        return std::shared_ptr<Object>(nullptr);
    // trim
    while(tokens[from]==" " && from<to)
        from += 1;
    while(tokens[to]==" " && to>from)
        to -= 1;
    if(from>to)
        return std::shared_ptr<Object>(nullptr);
    if(tokens[from]=="'" && tokens[to]=="'") {
        bool broken = false;
        for(int i=from+1;i<to;i++)
            if(tokens[i]=="'") {
                broken = true;
                break;
            }
        if(!broken){
            std::string str = "";
            for(int i=from+1;i<to;i++)
                str += tokens[i];
            return std::make_shared<String>(str);
        }
    }
    if(tokens[from]=="{" && tokens[to]=="}") {
        int depth = 0;
        bool broken = false;
        for(int i=from;i<to;i++) {
            std::string c = tokens[i];
            if(c=="{")
               depth += 1;
            if(c=="}")
               depth -= 1;
            if(depth==0){
                broken = true;
                break;
            }
        }
        if(!broken)
            return std::make_shared<Scoped>(parse(tokens, from+1, to-1));
    }
    if(tokens[from]=="(" && tokens[to]==")"){
        int depth = 0;
        bool broken = false;
        for(int i=from;i<to;i++) {
            std::string c = tokens[i];
            if(c=="(")
               depth += 1;
            if(c==")")
               depth -= 1;
            if(depth==0){
                broken = true;
                break;
            }
        }
        if(!broken)
            return std::make_shared<Unscoped>(parse(tokens, from+1, to-1));
    }
    int depth = 0;
    for(int i=from;i<=to;i++) {
        std::string c = tokens[i];
        if(c=="{" || c=="(" || c=="[")
           depth += 1;
        if(c=="}" || c==")" || c=="]")
            depth -= 1;
        if(!depth && c==";") {
            auto first = parse(tokens, from, i-1);
            auto second = parse(tokens, i+1, to);
            if(first==nullptr && second==nullptr)
                continue;
            if(first==nullptr)
                return second;
            if(second==nullptr)
                return first;
            return std::make_shared<Sequence>(first, second);
        }
    }
    int pos_access = -1;
    for(int i=from;i<=to;i++) {
        std::string c = tokens[i];
        if(c=="{" || c=="(" || c=="[")
           depth += 1;
        if(c=="}" || c==")" || c=="]")
            depth -= 1;
        if(depth)
            continue;
        if(tokens[i]==":=")
            return std::make_shared<Callable>(predicate_parts(tokens, from, i-1), parse(tokens, i+1, to));
        if(tokens[i]=="=")
            return std::make_shared<Assign>(parse(tokens, from, i-1), parse(tokens, i+1, to));
        if(tokens[i]=="."){
            pos_access = i;
            if(pos_access!=-1)
                return std::make_shared<Access>(parse(tokens, from, pos_access-1), parse(tokens, pos_access+1, to));
        }
        if(tokens[i]==":"){
            pos_access = i;
            if(pos_access!=-1)
                return std::make_shared<Edit>(parse(tokens, from, pos_access-1), parse(tokens, pos_access+1, to));
        }
        if(tokens[i]=="#"){
            pos_access = i;
            if(pos_access!=-1)
                return std::make_shared<Fallfront>(parse(tokens, from, pos_access-1), parse(tokens, pos_access+1, to));
        }
    }

    // convert to variable if only one token remaining
    if(from==to) {
        if(is_number_convertible(tokens[from]))
            return std::make_shared<Number>(std::stof(tokens[from]));
        //return std::make_shared<Variable>(tokens[from]);
    }
    auto parts = predicate_parts(tokens, from, to);
    return std::make_shared<Predicate>(parts);
}

std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string> tokens) {
    return predicate_parts(tokens, 0, tokens.size()-1);
}

std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string>& tokens, int from, int to) {
    // trim
    while(tokens[from]==" " && from<to)
        from += 1;
    while(tokens[to]==" " && to>from)
        to -= 1;
    std::vector<std::shared_ptr<PredicatePart>> predicateParts;
    int depth = 0;
    int block_start = -1;
    for(int i=from;i<=to;i++) {
        if(tokens[i]==" " || tokens[i]=="")
            continue;
        std::string token = tokens[i];
        if(token=="(" || token=="{" || token=="[") {
            depth += 1;
            //predicateParts.push_back(std::make_shared<PredicatePartWord>(token));
            if(depth==1)
                block_start = i;
        }
        else if(token==")" || token=="}" || token=="]") {
            depth -= 1;
            if(depth==0) {
                predicateParts.push_back(std::make_shared<PredicatePartObject>(parse(tokens, block_start, i)));  // include edges to differentiate between types of blocks
                block_start = -1;
            }
            //predicateParts.push_back(std::make_shared<PredicatePartWord>(token));
        }
        else if(depth==0) {
            if(is_number_convertible(token))
                predicateParts.push_back(std::make_shared<PredicatePartObject>(std::make_shared<Number>(std::stof(token))));
            else
                predicateParts.push_back(std::make_shared<PredicatePartWord>(token));
        }
    }
    if(block_start!=-1 || depth>0)
        error("Imbalanced parentheses or brackets.");
    return predicateParts;
}


std::shared_ptr<Object> parse(const std::string& source) {
    std::vector<std::string> tokens = tokenize(source);
    std::shared_ptr<Object> program = parse(tokens, 0, tokens.size()-1);
    return program;
}

