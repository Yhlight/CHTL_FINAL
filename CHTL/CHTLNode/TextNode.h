#pragma once

#include "Node.h"
#include <string>

namespace CHTL {

class TextNode : public Node {
public:
    std::string value;

    std::string ToString() const override;
};

} // namespace CHTL
