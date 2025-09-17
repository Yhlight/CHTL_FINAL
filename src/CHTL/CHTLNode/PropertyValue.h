#ifndef CHTL_PROPERTY_VALUE_H
#define CHTL_PROPERTY_VALUE_H

#include <variant>
#include <string>
#include <vector>
#include "../CHTLLexer/Token.h"
#include "PropertyReferenceNode.h"
#include "ResponsiveValueNode.h"

namespace CHTL {

class ElementNode; // Forward-declaration needed by UnresolvedProperty

struct Value {
    double Dvalue;
    std::string Svalue;
    std::string stringValue;
};

using PropertyValue = std::variant<Token, PropertyReferenceNode, ResponsiveValueNode>;

// Struct to hold info about a property that needs late resolution
struct UnresolvedProperty {
    ElementNode* element; // Pointer to the element node
    std::string property_name;
    std::vector<PropertyValue> value_parts;
};


} // namespace CHTL

#endif // CHTL_PROPERTY_VALUE_H
