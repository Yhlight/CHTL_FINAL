#pragma once

#include "Object.h"

namespace CHTL {

class BooleanObject : public Object {
public:
    BooleanObject(bool value) : value(value) {}

    ObjectType Type() const override { return ObjectType::BOOLEAN_OBJ; }
    std::string Inspect() const override { return value ? "true" : "false"; }

    bool value;
};

} // namespace CHTL
