#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL
{
    class ElementNode : public BaseNode
    {
    public:
        std::string tagName;
        std::map<std::string, std::string> attributes;
        std::vector<std::unique_ptr<BaseNode>> children;

        ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

        void addChild(std::unique_ptr<BaseNode> child);
        void addAttribute(const std::string& key, const std::string& value);

        NodeType getType() const override { return NodeType::ELEMENT_NODE; }
        std::string toString(int indent = 0) const override;
    };
}

#endif // CHTL_ELEMENTNODE_H
