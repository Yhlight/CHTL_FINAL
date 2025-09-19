#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include "../Expression/ResponsiveValueNode.h"

namespace CHTL {

// An attribute can have a static string value or a dynamic responsive value.
struct HtmlAttribute {
    std::string key;
    std::variant<std::string, ResponsiveValueNode> value;
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
