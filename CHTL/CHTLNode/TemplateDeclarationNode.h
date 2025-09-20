#ifndef TEMPLATE_DECLARATION_NODE_H
#define TEMPLATE_DECLARATION_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include "TemplateDefinitionNode.h" // We'll store the definition here
#include <string>
#include <memory>

namespace CHTL {

// Represents a [Template] declaration in the source code.
// This node will exist in the AST.
class TemplateDeclarationNode : public BaseNode {
public:
    explicit TemplateDeclarationNode(std::unique_ptr<TemplateDefinitionNode> def)
        : definition(std::move(def)) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::unique_ptr<TemplateDefinitionNode> definition;
};

} // namespace CHTL

#endif // TEMPLATE_DECLARATION_NODE_H
