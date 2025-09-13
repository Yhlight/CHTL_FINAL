#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <string>
#include <memory>
#include <vector>
#include <variant>

#include <map>

namespace CHTL {
namespace CHTLJS {

enum class JSNodeType {
    Script, // Represents a block of standard JS
    EnhancedSelector,
    Listen
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

class ListenNode : public JSNode {
public:
    explicit ListenNode(std::unique_ptr<JSNode> target) : target_(std::move(target)) {}
    JSNodeType getType() const override { return JSNodeType::Listen; }

    std::unique_ptr<JSNode> target_;
    std::map<std::string, std::string> events; // event name -> callback body
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_NODE_H
