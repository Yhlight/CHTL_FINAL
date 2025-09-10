#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// Represents an element in CHTL, like `div`, `span`, `html`, etc.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    // Constructor to initialize the element with its tag name.
    explicit ElementNode(const std::string& name) : tagName(name) {}

    // Helper method to add a child node.
    // This takes ownership of the node.
    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    // Helper method to add an attribute.
    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }
};

#endif // CHTL_ELEMENT_NODE_H
