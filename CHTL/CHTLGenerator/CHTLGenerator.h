#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <sstream>
#include <map>

namespace CHTL {

class ExpressionEvaluator; // Forward declaration

struct CompilationResult {
    std::string html;
    std::string css;
    std::string js;
};

class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, const std::map<std::string, std::string>& placeholders = {});
    CompilationResult generate(BaseNode* root);

    void visit(DocumentNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ScriptNode& node) override;
    std::map<std::string, AttributeNode> computeFinalStyles(ElementNode& node);

private:
    void collectAndGenerateCss(BaseNode* node, ExpressionEvaluator& evaluator);
    void collectJs(BaseNode* node);

    std::stringstream html_output;
    std::stringstream css_output;
    std::stringstream js_output;
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    const std::map<std::string, std::string> placeholders;
    BaseNode* doc_root = nullptr;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
