#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator();

    // Generates an HTML string from a vector of root AST nodes.
    std::string generate(const std::vector<std::unique_ptr<Node>>& roots);

private:
    std::stringstream m_output;
    std::stringstream m_global_css;
    int m_indent_level;

    // Main recursive visitor function.
    void visit(const Node* node);

    // Specific visitor functions for each node type.
    void visitElement(const ElementNode* node);
    void visitText(const TextNode* node);

    // Helper for indentation.
    std::string getIndentation() const;
};

} // namespace CHTL
