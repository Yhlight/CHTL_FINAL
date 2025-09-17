#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"

#include <string>
#include <vector>
#include <sstream>
#include <memory>

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator();
    std::string Generate(Program* program);

private:
    void generateNode(BaseNode* node);
    void generateElementNode(ElementNode* node);
    void generateTextNode(TextNode* node);
    void generateCommentNode(CommentNode* node);
    void generateAttributeNode(AttributeNode* node, std::vector<std::string>& attributes);
    void generateStyleNode(StyleNode* node, std::vector<std::string>& style_properties);

    std::stringstream m_output;
    int m_indent_level;

    void indent();
};

} // namespace CHTL
