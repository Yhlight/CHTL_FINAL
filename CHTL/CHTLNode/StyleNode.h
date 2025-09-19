#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include "CssRuleNode.h"
#include "Visitor.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class TemplateDefinitionNode;

class StyleNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    struct StyleApplication {
        std::shared_ptr<const TemplateDefinitionNode> definition = nullptr;
        std::vector<std::string> deleted_properties;
        std::vector<AttributeNode> new_or_overridden_properties;

        StyleApplication clone() const {
            StyleApplication new_app;
            new_app.definition = definition;
            new_app.deleted_properties = deleted_properties;
            for (const auto& prop : new_or_overridden_properties) {
                new_app.new_or_overridden_properties.push_back(prop.clone());
            }
            return new_app;
        }
    };

    std::vector<AttributeNode> direct_properties;
    std::vector<StyleApplication> template_applications;
    std::vector<CssRuleNode> global_rules;
};

} // namespace CHTL

#endif // STYLE_NODE_H
