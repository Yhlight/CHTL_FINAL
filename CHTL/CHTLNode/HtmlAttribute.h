#ifndef HTML_ATTRIBUTE_H
#define HTML_ATTRIBUTE_H

#include <string>

namespace CHTL {

// Represents a standard HTML attribute with a simple string value.
struct HtmlAttribute {
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // HTML_ATTRIBUTE_H
