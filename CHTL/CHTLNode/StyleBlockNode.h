#ifndef CHTL_STYLEBLOCKNODE_H
#define CHTL_STYLEBLOCKNODE_H

#include "BaseNode.h"
#include "CSSPropertyNode.h"
#include "CSSRuleNode.h"
#include <vector>
#include <memory>
#include <sstream>

class StyleBlockNode : public BaseNode {
public:
    // Properties that apply directly as inline styles
    std::vector<std::unique_ptr<CSSPropertyNode>> inline_properties;
    // CSS rules that go into the global stylesheet
    std::vector<std::unique_ptr<CSSRuleNode>> rules;

    // Constructor
    explicit StyleBlockNode(const Token& token) {
        this->token = token; // The 'style' keyword token
    }

    void addProperty(std::unique_ptr<CSSPropertyNode> prop) {
        inline_properties.push_back(std::move(prop));
    }

    void addRule(std::unique_ptr<CSSRuleNode> rule) {
        rules.push_back(std::move(rule));
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "StyleBlockNode:\n";
        for (const auto& prop : inline_properties) {
            ss << prop->debugString(indent + 1);
        }
        for (const auto& rule : rules) {
            ss << rule->debugString(indent + 1);
        }
        return ss.str();
    }
};

#endif //CHTL_STYLEBLOCKNODE_H
