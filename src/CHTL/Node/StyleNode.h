#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "CssProperty.h"
#include <vector>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    std::vector<CssProperty> properties;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H
