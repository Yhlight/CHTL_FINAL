#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <sstream>
#include <map>

namespace CHTL {

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
    void visit(ConfigNode& node) override;

private:
    std::stringstream html_output;
    std::stringstream css_output;
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root = nullptr;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
