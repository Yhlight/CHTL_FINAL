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
};

class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates);
    CompilationResult generate(BaseNode* root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;

private:
    void collectAndGenerateCss(BaseNode* node, ExpressionEvaluator& evaluator);

    std::stringstream html_output;
    std::stringstream css_output;
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root = nullptr;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
