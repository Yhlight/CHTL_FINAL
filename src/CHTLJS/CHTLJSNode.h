#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <string>
#include <memory>
#include <vector>
#include <variant>

namespace CHTL {
namespace CHTLJS {

enum class JSNodeType {
    Script, // Represents a block of standard JS
    EnhancedSelector
};

class JSNode {
public:
    virtual ~JSNode() = default;
    virtual JSNodeType getType() const = 0;
};

class StandardJSNode : public JSNode {
public:
    explicit StandardJSNode(std::string content) : content_(std::move(content)) {}
    JSNodeType getType() const override { return JSNodeType::Script; }
    std::string content_;
};

class EnhancedSelectorNode : public JSNode {
public:
    explicit EnhancedSelectorNode(std::string selector) : selector_(std::move(selector)) {}
    JSNodeType getType() const override { return JSNodeType::EnhancedSelector; }
    std::string selector_;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_NODE_H
