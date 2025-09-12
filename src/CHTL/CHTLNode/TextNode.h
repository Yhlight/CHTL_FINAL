#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    explicit TextNode(const std::string& content) : content(content) {}
};

} // namespace CHTL
