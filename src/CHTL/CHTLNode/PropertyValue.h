#ifndef CHTL_PROPERTY_VALUE_H
#define CHTL_PROPERTY_VALUE_H

#include <variant>
#include <string>
#include <memory>
#include "../CHTLLexer/Token.h"
#include "PropertyReferenceNode.h"

namespace CHTL {

// Forward-declare expression node types to break circular dependency
struct ArithmeticNode;
struct ConditionalNode;

// Represents the result of a partially or fully evaluated expression
struct Value {
    double Dvalue = 0.0;
    std::string Svalue;
    std::string stringValue;
};

// A property's value can be a simple token, a reference to another property,
// or a complex expression tree.
using PropertyValue = std::variant<
    Token,
    PropertyReferenceNode,
    std::unique_ptr<ArithmeticNode>,
    std::unique_ptr<ConditionalNode>
>;

} // namespace CHTL

#endif // CHTL_PROPERTY_VALUE_H
