#pragma once

#include "CHTLNode/AstVisitor.hpp"
#include "CHTLNode/BaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

class AstPrinter : public AstVisitor {
public:
    std::string print(const std::vector<NodePtr>& nodes);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(TemplateDefinitionNode& node) override;

private:
    void parenthesize(const std::string& name, const std::vector<NodePtr>& nodes);
    void parenthesize_attr(const std::string& name, const std::vector<std::shared_ptr<AttributeNode>>& nodes);

    std::string m_result;
};

} // namespace CHTL
