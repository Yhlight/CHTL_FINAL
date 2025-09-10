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
    // The body of the style block can contain properties, rules, or other statements.
    std::vector<std::unique_ptr<BaseNode>> body;

    // Constructor
    explicit StyleBlockNode(const Token& token) {
        this->token = token; // The 'style' keyword token
    }

    void addNode(std::unique_ptr<BaseNode> node) {
        body.push_back(std::move(node));
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "StyleBlockNode:\n";
        for (const auto& node : body) {
            ss << node->debugString(indent + 1);
        }
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<StyleBlockNode>(token);
        for (const auto& node : body) {
            clonedNode->addNode(node->clone());
        }
        return clonedNode;
    }
};

#endif //CHTL_STYLEBLOCKNODE_H
