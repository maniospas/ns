#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Object.h>

class Sequence: public Object {
    private:
        std::shared_ptr<Object> first_;
        std::shared_ptr<Object> second_;
    public:
        Sequence(std::shared_ptr<Object> first, std::shared_ptr<Object> second);
        virtual ~Sequence();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
};

#endif // SEQUENCE_H
