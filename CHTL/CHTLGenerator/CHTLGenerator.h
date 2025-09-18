#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

// The CHTLGenerator traverses the AST using the Visitor pattern
// and generates the final HTML output.
class CHTLGenerator : public Visitor {
public:
    // The main entry point. It takes the root of the AST and returns the generated code.
    std::string generate(BaseNode* root);

    // Visitor methods for each concrete node type.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;

private:
    std::stringstream output;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
