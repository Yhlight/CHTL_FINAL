#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <unordered_map>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    const std::string& getTagName() const;
    void setAttribute(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getAttributes() const;

private:
    std::string m_tagName;
    std::unordered_map<std::string, std::string> m_attributes;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H
