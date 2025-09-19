#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// A temporary struct for HTML attributes until they are refactored
// to handle expressions like style properties.
struct HtmlAttribute {
    std::string key;
    std::string value;
};

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName);
    void accept(Visitor& visitor) override;
    void addChild(std::unique_ptr<BaseNode> child);
    void addAttribute(const HtmlAttribute& attr);
    std::unique_ptr<BaseNode> clone() const override;

    std::string tagName;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::vector<HtmlAttribute> attributes;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
