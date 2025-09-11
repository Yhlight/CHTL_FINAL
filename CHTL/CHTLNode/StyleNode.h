#pragma once

#include "Node.h"
#include "CSSPropertyNode.h"
#include "StyleSelectorNode.h"
#include "TemplateUsageNode.h"
#include <vector>
#include <sstream>

namespace CHTL {

class StyleNode : public Node {
public:
    StyleNode() = default;

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "StyleNode [\n";
        if (!inline_properties.empty()) {
            ss << std::string(indent + 2, ' ') << "Inline Properties:\n";
            for (const auto& prop : inline_properties) {
                ss << prop->ToString(indent + 4) << "\n";
            }
        }
        if (!selector_blocks.empty()) {
            ss << std::string(indent + 2, ' ') << "Selector Blocks:\n";
            for (const auto& sel : selector_blocks) {
                ss << sel->ToString(indent + 4) << "\n";
            }
        }
        if (!template_usages.empty()) {
            ss << std::string(indent + 2, ' ') << "Template Usages:\n";
            for (const auto& usage : template_usages) {
                ss << usage->ToString(indent + 4) << "\n";
            }
        }
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    NodePtr clone() const override {
        auto new_style_node = std::make_shared<StyleNode>();
        for (const auto& prop : inline_properties) {
            new_style_node->inline_properties.push_back(std::dynamic_pointer_cast<CSSPropertyNode>(prop->clone()));
        }
        for (const auto& sel : selector_blocks) {
            new_style_node->selector_blocks.push_back(std::dynamic_pointer_cast<StyleSelectorNode>(sel->clone()));
        }
        for (const auto& usage : template_usages) {
            new_style_node->template_usages.push_back(std::dynamic_pointer_cast<TemplateUsageNode>(usage->clone()));
        }
        return new_style_node;
    }

    CSSPropertyList inline_properties;
    StyleSelectorList selector_blocks;
    NodeList template_usages;
};

} // namespace CHTL
