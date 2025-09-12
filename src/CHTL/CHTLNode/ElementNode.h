#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include "ExprNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    void accept(NodeVisitor& visitor) override;

    const std::string& getTagName() const { return tagName; }
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    const std::map<std::string, std::unique_ptr<ExprNode>>& getInlineStyles() const { return inlineStyles; }
    void addInlineStyle(const std::string& key, std::unique_ptr<ExprNode> value) {
        inlineStyles[key] = std::move(value);
    }

    const std::string& getPrimarySelector() const { return primarySelector; }
    void setPrimarySelector(const std::string& selector) {
        primarySelector = selector;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::unique_ptr<ExprNode>> inlineStyles;
    std::string primarySelector;
};

} // namespace CHTL

#endif // CHTL_ELEMENTNODE_H
