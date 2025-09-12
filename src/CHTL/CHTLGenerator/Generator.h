#pragma once

#include "CHTL/CHTLNode/AST.h"
#include <string>
#include <sstream>
#include <memory>

namespace CHTL {

class Generator {
public:
    Generator();
    std::string generate(const ProgramNode& program);

private:
    void generateNode(const std::shared_ptr<Node>& node);
    void generateElement(const std::shared_ptr<ElementNode>& node);
    void generateText(const std::shared_ptr<TextNode>& node);
    void generateComment(const std::shared_ptr<CommentNode>& node);

    void write(const std::string& content);
    void writeLine(const std::string& line);
    std::string getIndent();

    std::stringstream m_out;
    int m_indentLevel = 0;
    const std::string m_indentStr = "  "; // 2 spaces for indentation
};

} // namespace CHTL
