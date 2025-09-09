#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

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
    const std::vector<std::unique_ptr<Node>>* m_roots;

    // Main recursive visitor function.
    void visit(const Node* node);

    // Expression evaluator
    std::string evaluateExpression(const ExpressionNode* expr, const std::map<std::string, std::string>& context);

    // Specific visitor functions for each node type.
    void visitElement(const ElementNode* node);
    void visitText(const TextNode* node);

    // Selector engine
    const ElementNode* findNode(const std::string& selector) const;
    const ElementNode* findNodeRecursive(const std::string& selector, const Node* current_node) const;

    // Helper for indentation.
    std::string getIndentation() const;
};

} // namespace CHTL
