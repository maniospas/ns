#ifndef STRING_H
#define STRING_H

#include <string>
#include <Object.h>

class String: public Object
{
    private:
        std::string value_;
    public:
        String(const std::string& value);
        virtual ~String();
        const std::string name() const;
        std::shared_ptr<Object> value(std::shared_ptr<Scope> scope);
        const float value() const;
};

#endif // STRING_H
