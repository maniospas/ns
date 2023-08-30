#ifndef PREDICATEPART_H
#define PREDICATEPART_H

#include <string>
#include <memory>
#include "Object.h"

class PredicatePart {
    public:
        PredicatePart();
        virtual ~PredicatePart();
        virtual std::string name() const = 0;
        virtual std::string assignment_name() const = 0;
        virtual std::shared_ptr<Object> object() const = 0;
};

class PredicatePartWord: public PredicatePart {
    private:
        std::string name_;
    public:
        PredicatePartWord(const std::string& name);
        std::string name() const;
        std::string assignment_name() const;
        std::shared_ptr<Object> object() const;
};

class PredicatePartObject: public PredicatePart {
    private:
        std::shared_ptr<Object> object_;
    public:
        PredicatePartObject(std::shared_ptr<Object> object);
        std::string name() const;
        std::string assignment_name() const;
        std::shared_ptr<Object> object() const;
};

#endif
