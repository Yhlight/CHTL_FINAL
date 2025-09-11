#pragma once

#include <string>
#include <memory>

namespace CHTL {

enum class ObjectType {
    NUMBER_OBJ,
    BOOLEAN_OBJ,
    STRING_OBJ,
    NULL_OBJ,
};

class Object {
public:
    virtual ~Object() = default;
    virtual ObjectType Type() const = 0;
    virtual std::string Inspect() const = 0;
    virtual std::string RawValue() const { return Inspect(); }
};

using ObjectPtr = std::shared_ptr<Object>;

} // namespace CHTL
