#ifndef CHTL_CSSRULENODE_H
#define CHTL_CSSRULENODE_H

#include "BaseNode.h"
#include "CSSPropertyNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class CSSRuleNode : public BaseNode {
public:
    std::string selector;
    std::vector<std::unique_ptr<CSSPropertyNode>> properties;

    // Constructor
    CSSRuleNode(const Token& token, std::string selector)
        : selector(std::move(selector)) {
        this->token = token;
    }

    void addProperty(std::unique_ptr<CSSPropertyNode> prop) {
        properties.push_back(std::move(prop));
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "CSSRuleNode: " << selector << " {\n";
        for (const auto& prop : properties) {
            ss << prop->debugString(indent + 1);
        }
        ss << indentString(indent) << "}\n";
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<CSSRuleNode>(token, selector);
        for (const auto& prop : properties) {
            clonedNode->addProperty(std::unique_ptr<CSSPropertyNode>(static_cast<CSSPropertyNode*>(prop->clone().release())));
        }
        return clonedNode;
    }
};

#endif //CHTL_CSSRULENODE_H
