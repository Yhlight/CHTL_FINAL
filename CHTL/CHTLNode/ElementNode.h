#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include "../Expression/Expr.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// A temporary struct for HTML attributes until they are refactored
// to handle expressions like style properties.
struct HtmlAttribute {
    std::string key;
    std::string value;
    std::unique_ptr<Expr> value_expr = nullptr;
    
    // Default constructor
    HtmlAttribute() = default;
    
    // Constructor for simple attributes
    HtmlAttribute(const std::string& k, const std::string& v) 
        : key(k), value(v), value_expr(nullptr) {}
    
    // Constructor for attributes with expressions
    HtmlAttribute(const std::string& k, const std::string& v, std::unique_ptr<Expr> expr) 
        : key(k), value(v), value_expr(std::move(expr)) {}
    
    // Copy constructor
    HtmlAttribute(const HtmlAttribute& other) 
        : key(other.key), value(other.value), 
          value_expr(other.value_expr ? other.value_expr->clone() : nullptr) {}
    
    // Move constructor
    HtmlAttribute(HtmlAttribute&& other) noexcept 
        : key(std::move(other.key)), value(std::move(other.value)), 
          value_expr(std::move(other.value_expr)) {}
    
    // Copy assignment operator
    HtmlAttribute& operator=(const HtmlAttribute& other) {
        if (this != &other) {
            key = other.key;
            value = other.value;
            value_expr = other.value_expr ? other.value_expr->clone() : nullptr;
        }
        return *this;
    }
    
    // Move assignment operator
    HtmlAttribute& operator=(HtmlAttribute&& other) noexcept {
        if (this != &other) {
            key = std::move(other.key);
            value = std::move(other.value);
            value_expr = std::move(other.value_expr);
        }
        return *this;
    }
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
    std::vector<std::string> constraints;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
