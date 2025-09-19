#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class PlaceholderNode : public BaseNode {
public:
    explicit PlaceholderNode(const std::string& placeholder);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
    std::string placeholder;
};

} // namespace CHTL
