#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

#include <string>
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

    std::stringstream m_output;
    int m_indent_level;

    void indent();
};

} // namespace CHTL
