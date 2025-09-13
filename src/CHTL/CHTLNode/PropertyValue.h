#ifndef CHTL_PROPERTY_VALUE_H
#define CHTL_PROPERTY_VALUE_H

#include <variant>
#include <string>
#include "../CHTLLexer/Token.h"
#include "PropertyReferenceNode.h"
#include "ResponsiveValueNode.h"

namespace CHTL {

struct Value {
    double Dvalue;
    std::string Svalue;
    std::string stringValue;
};

using PropertyValue = std::variant<Token, PropertyReferenceNode, ResponsiveValueNode>;

} // namespace CHTL

#endif // CHTL_PROPERTY_VALUE_H
