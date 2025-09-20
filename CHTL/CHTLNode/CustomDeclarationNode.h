#ifndef CUSTOM_DECLARATION_NODE_H
#define CUSTOM_DECLARATION_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include "TemplateDefinitionNode.h" // Re-using this for the definition part
#include <string>
#include <memory>

namespace CHTL {

// Represents a [Custom] declaration in the source code.
class CustomDeclarationNode : public BaseNode {
public:
    explicit CustomDeclarationNode(std::unique_ptr<TemplateDefinitionNode> def)
        : definition(std::move(def)) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::unique_ptr<TemplateDefinitionNode> definition;
};

} // namespace CHTL

#endif // CUSTOM_DECLARATION_NODE_H
