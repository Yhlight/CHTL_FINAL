#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
