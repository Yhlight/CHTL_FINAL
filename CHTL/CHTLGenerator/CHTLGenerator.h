#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <sstream>
#include <map>
#include <memory>

namespace CHTL {

struct CompilationResult {
    std::string html;
    std::string css;
};

class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& templates);
    CompilationResult generate(BaseNode* root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(TemplateDeclarationNode& node) override;
    void visit(CustomDeclarationNode& node) override;
    void visit(ImportNode& node) override;

private:
    const std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& templates;
    BaseNode* doc_root = nullptr;
    std::stringstream html_output;
    std::stringstream css_output;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
