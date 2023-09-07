#ifndef CUSTOMPREDICATEEXECUTOR_H
#define CUSTOMPREDICATEEXECUTOR_H

#include <string>
#include <vector>
#include <memory>
#include "Object.h"
#include "Predicate.h"
#include "PredicatePart.h"
#include "Scope.h"

class CustomPredicateExecutor: public Object {
    private:
        std::vector<std::shared_ptr<PredicatePart>> names_;
    public:
        int priority = 0;
        std::vector<std::shared_ptr<PredicatePart>> match(
            std::vector<std::shared_ptr<PredicatePart>>& names,
            int& first_predicate_position);
        CustomPredicateExecutor(std::vector<std::shared_ptr<PredicatePart>> names);
        CustomPredicateExecutor(const std::string& text);
        virtual ~CustomPredicateExecutor();
        virtual std::shared_ptr<Scope> scoped(std::shared_ptr<Scope> scope);
        virtual std::shared_ptr<Scope> descoped(std::shared_ptr<Scope> scope);
        virtual std::shared_ptr<Scope> vscoped(std::shared_ptr<Scope> scope);
        virtual void evaluate_argument(
            std::shared_ptr<Scope> derived_scope,
            std::shared_ptr<Object> prototype, 
            std::shared_ptr<Object> expression, 
            std::shared_ptr<Scope> value_scope
        );
        const std::string name() const;
        const std::string assignment_name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        virtual std::shared_ptr<Scope> evaluate_all_arguments(std::shared_ptr<Scope> scope, std::shared_ptr<Predicate> predicate);
        virtual std::shared_ptr<Object> call(std::shared_ptr<Scope> scope, std::shared_ptr<Predicate> predicate, std::shared_ptr<Scope> derived_scope);
        virtual std::shared_ptr<Object> implement(std::shared_ptr<Scope> scope) = 0;
        static std::shared_ptr<Scope> predicateScope;
        virtual bool can_call(std::shared_ptr<Scope> scope, std::shared_ptr<Predicate> predicate, std::shared_ptr<Scope> derived_scope) ;
};

#endif
