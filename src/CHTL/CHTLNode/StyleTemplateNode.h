#ifndef CHTL_STYLE_TEMPLATE_NODE_H
#define CHTL_STYLE_TEMPLATE_NODE_H

#include <string>
#include <vector>
#include <utility>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// This is a storage class for the parser, not a part of the main AST tree.
class StyleTemplateNode {
public:
    explicit StyleTemplateNode(const std::string& name) : name_(name) {}

    std::string name_;
    std::vector<std::pair<std::string, std::vector<Token>>> properties_;
};

} // namespace CHTL

#endif // CHTL_STYLE_TEMPLATE_NODE_H
