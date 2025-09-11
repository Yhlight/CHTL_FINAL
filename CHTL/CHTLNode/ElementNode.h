#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit ElementNode(const std::string& name) : tagName(name) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }
};

#endif // CHTL_ELEMENT_NODE_H
