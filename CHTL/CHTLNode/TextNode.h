#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    TextNode(const std::string& content);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    const std::string& getContent() const;

private:
    std::string m_content;
};

} // namespace CHTL

#endif // TEXT_NODE_H
