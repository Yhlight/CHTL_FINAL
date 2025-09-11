#pragma once

#include "Object.h"

namespace CHTL {

class StringObject : public Object {
public:
    StringObject(std::string value) : value(std::move(value)) {}

    ObjectType Type() const override { return ObjectType::STRING_OBJ; }
    std::string Inspect() const override { return "\"" + value + "\""; }
    std::string RawValue() const override { return value; }

    std::string value;
};

} // namespace CHTL
