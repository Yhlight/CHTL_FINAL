#ifndef CHTL_PROPERTY_VALUE_H
#define CHTL_PROPERTY_VALUE_H

#include <string>
#include <memory>
#include "ExpressionNode.h"

namespace CHTL {

// Represents a single CSS property declaration (e.g., "color: red;")
struct PropertyDeclaration {
    std::string name;
    std::unique_ptr<ExpressionNode> value;

    PropertyDeclaration(std::string n, std::unique_ptr<ExpressionNode> v)
        : name(std::move(n)), value(std::move(v)) {}
};

} // namespace CHTL

#endif // CHTL_PROPERTY_VALUE_H
