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
};

#endif //CHTL_CSSRULENODE_H
