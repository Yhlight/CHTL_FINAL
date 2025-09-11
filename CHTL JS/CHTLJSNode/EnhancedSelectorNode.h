#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL_JS {

class EnhancedSelectorNode : public Node {
public:
    EnhancedSelectorNode(std::string selector) : selector(std::move(selector)) {}

    std::string ToString() const override {
        return "EnhancedSelector({{" + selector + "}})";
    }

    std::string selector;
};

} // namespace CHTL_JS
