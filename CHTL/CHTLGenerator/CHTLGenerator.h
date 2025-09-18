#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

struct CompilationResult {
    std::string html;
    std::string css;
};

// The CHTLGenerator traverses the AST using the Visitor pattern
// and generates the final HTML and CSS output.
class CHTLGenerator : public Visitor {
public:
    // The main entry point. It takes the root of the AST and returns the generated code.
    CompilationResult generate(BaseNode* root);

    // Visitor methods for each concrete node type.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;

private:
    std::stringstream html_output;
    std::stringstream css_output;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
