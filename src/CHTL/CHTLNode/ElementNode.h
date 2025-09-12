#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::map<std::string, std::string> styles;

    // For the synthetic root node to collect global styles
    std::map<std::string, std::string> global_styles;

    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    void addAttribute(std::shared_ptr<AttributeNode> attribute) {
        attributes.push_back(attribute);
    }

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }
};

} // namespace CHTL
