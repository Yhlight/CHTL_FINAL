#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string key;
    std::shared_ptr<BaseNode> value;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_NODE_H
