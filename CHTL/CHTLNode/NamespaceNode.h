#ifndef CHTL_NAMESPACENODE_H
#define CHTL_NAMESPACENODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> body;

    // Constructor
    NamespaceNode(const Token& token, std::string name)
        : name(std::move(name)) {
        this->token = token; // The '[Namespace]' keyword token
    }

    void addNode(std::unique_ptr<BaseNode> node) {
        body.push_back(std::move(node));
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "NamespaceNode: " << name << "\n";
        for (const auto& node : body) {
            ss << node->debugString(indent + 1);
        }
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<NamespaceNode>(token, name);
        for (const auto& node : body) {
            clonedNode->addNode(node->clone());
        }
        return clonedNode;
    }
};

#endif //CHTL_NAMESPACENODE_H
