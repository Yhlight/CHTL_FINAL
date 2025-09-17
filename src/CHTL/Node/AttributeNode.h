#ifndef CHTL_ATTRIBUTE_NODE_H
#define CHTL_ATTRIBUTE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_ATTRIBUTE_NODE_H
