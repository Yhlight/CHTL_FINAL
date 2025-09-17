#ifndef CHTL_CSS_PROPERTY_NODE_H
#define CHTL_CSS_PROPERTY_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CssPropertyNode : public BaseNode {
public:
    std::string key;
    std::shared_ptr<BaseNode> value;
};

} // namespace CHTL

#endif // CHTL_CSS_PROPERTY_NODE_H
