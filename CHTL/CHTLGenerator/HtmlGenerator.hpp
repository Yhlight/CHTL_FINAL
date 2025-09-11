#pragma once

#include "../CHTLNode/AstVisitor.hpp"
#include "../CHTLNode/BaseNode.hpp"
#include "../CHTLNode/TemplateDefinitionNode.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

class HtmlGenerator : public AstVisitor {
public:
    std::string generate(const std::vector<NodePtr>& nodes);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(AttributeNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(TemplateDefinitionNode& node) override;

private:
    std::string m_result;
    // Helper for indentation
    int m_indent_level = 0;
    void indent();

    // Storage for templates
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_styleTemplates;
};

} // namespace CHTL
