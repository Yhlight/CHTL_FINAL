#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::string content;
};

} // namespace CHTL
