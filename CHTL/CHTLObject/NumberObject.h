#pragma once

#include "Object.h"

namespace CHTL {

class NumberObject : public Object {
public:
    NumberObject(double value, std::string unit = "") : value(value), unit(std::move(unit)) {}

    ObjectType Type() const override { return ObjectType::NUMBER_OBJ; }
    std::string Inspect() const override { return std::to_string(value) + unit; }

    double value;
    std::string unit;
};

} // namespace CHTL
