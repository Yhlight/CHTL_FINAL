#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include "Attribute.h"
#include <string>
#include <vector>
#include <memory>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);

    void accept(Visitor& visitor) override;

    const std::string& getTagName() const;
    const std::vector<Attribute>& getAttributes() const;
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;

    void addAttribute(const Attribute& attribute);
    void addChild(std::unique_ptr<BaseNode> child);

private:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif // ELEMENT_NODE_H
