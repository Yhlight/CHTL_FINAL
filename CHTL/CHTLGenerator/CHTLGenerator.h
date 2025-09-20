#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CompilerDispatcher/CompilerDispatcher.h" // For CompilationResult
#include <string>
#include <sstream>
#include <map>

namespace CHTL {

// The CHTLGenerator traverses the AST using the Visitor pattern
// and generates the final HTML and CSS output.
class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, TemplateDefinitionNode>& templates);
    // The main entry point. It takes the root of the AST and returns the generated code.
    CompilationResult generate(BaseNode* root);

    // Visitor methods for each concrete node type.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(TemplateDeclarationNode& node) override;
    void visit(CustomDeclarationNode& node) override;
    void visit(ImportNode& node) override;

private:
    const std::map<std::string, TemplateDefinitionNode>& templates;
    BaseNode* doc_root = nullptr; // To provide context for the evaluator
    std::stringstream html_output;
    std::stringstream css_output;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
