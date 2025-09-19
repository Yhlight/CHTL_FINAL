#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include "CHTL/Expression/Expr.h" // Include for std::unique_ptr<Expr>
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

struct HtmlAttribute {
    std::string key;
    std::unique_ptr<Expr> value_expr;

    // Add a clone method for deep copying during template instantiation
    HtmlAttribute clone() const {
        return {key, value_expr ? value_expr->clone() : nullptr};
    }
};

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName);
    void accept(Visitor& visitor) override;
    void addChild(std::unique_ptr<BaseNode> child);
    void addAttribute(HtmlAttribute attr); // Pass by value to allow moving
    std::unique_ptr<BaseNode> clone() const override;

    std::string tagName;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::vector<HtmlAttribute> attributes;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
