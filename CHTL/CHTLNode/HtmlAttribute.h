#ifndef HTML_ATTRIBUTE_H
#define HTML_ATTRIBUTE_H

#include <string>
#include <memory>
#include "../Expression/Expr.h"

namespace CHTL {

// Represents an HTML attribute, which can have a complex expression as a value.
struct HtmlAttribute {
    std::string key;
    std::unique_ptr<Expr> value_expr;
};

} // namespace CHTL

#endif // HTML_ATTRIBUTE_H
