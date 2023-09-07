#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Object.h>
#include <vector>

class Sequence: public Object {
    private:
        std::vector<std::shared_ptr<Object>> expressions;
    public:
        Sequence(std::shared_ptr<Object> first, std::shared_ptr<Object> second);
        virtual ~Sequence();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        const std::string type() const {return "next command";};
        
        // vectorization
        void set(int i, std::shared_ptr<Object> value);
        std::shared_ptr<Object> get(int i);
        int size();
};

#endif // SEQUENCE_H
