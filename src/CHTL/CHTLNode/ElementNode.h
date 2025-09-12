#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include <string>
#include <map>

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

    const std::map<std::string, std::string>& getInlineStyles() const { return inlineStyles; }
    void addInlineStyle(const std::string& key, const std::string& value) {
        inlineStyles[key] = value;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> inlineStyles;
};

} // namespace CHTL

#endif // CHTL_ELEMENTNODE_H
