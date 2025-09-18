#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include <string>
#include <sstream>
#include <map>
#include <memory>

namespace CHTL {

class CssRuleNode; // Forward Declaration
class DocumentNode; // Forward Declaration

struct CompilationResult {
    std::string html;
    std::string css;
};

class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates);

    CompilationResult generate(BaseNode* root);

    // Visitor methods for AST nodes
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(CssRuleNode& node) override;
    void visit(DocumentNode& node) override;

private:
    void indent(int level);

    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& m_templates;
    BaseNode* m_doc_root = nullptr;
    std::unique_ptr<ExpressionEvaluator> m_evaluator;

    std::stringstream m_html_output;
    std::stringstream m_css_output;
    int m_indent_level = 0;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
