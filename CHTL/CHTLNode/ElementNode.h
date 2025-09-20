#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include "HtmlAttribute.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents an element node, which can have a tag name, attributes, and children.
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(const HtmlAttribute& attr) {
        attributes.push_back(attr);
    }

    std::string tagName;
    std::vector<HtmlAttribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
