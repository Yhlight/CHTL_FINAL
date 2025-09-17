#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H
