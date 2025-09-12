#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTL/CHTLNode/NodeVisitor.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <sstream>
#include "CHTL/CHTLContext/CHTLContext.h"

namespace CHTL {

// The Generator implements the NodeVisitor interface to traverse the AST.
class CHTLGenerator : public NodeVisitor {
public:
    std::string generate(BaseNode* root, CHTLContext& context);

    // We only need to implement the visitors for the nodes our MVP parser can create.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    CHTLContext* context = nullptr;
    std::stringstream output;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
