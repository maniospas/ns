#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <vector>
#include <string>
#include <memory>
#include "Object.h"
#include "PredicatePart.h"

void error(const std::string& message);
std::vector<std::string> tokenize(const std::string& source);
std::shared_ptr<Object> parse(std::vector<std::string>& tokens, int from, int to);
std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string> tokens);
std::vector<std::shared_ptr<PredicatePart>> predicate_parts(std::vector<std::string>& tokens, int from, int to);
std::shared_ptr<Object> parse(const std::string& source);

#endif
