#pragma once

#include "Node.h"
#include "CSSPropertyNode.h"
#include <string>
#include <utility>
#include <vector>
#include <sstream>

namespace CHTL {

class StyleSelectorNode : public Node {
public:
    StyleSelectorNode(std::string selector) : selector(std::move(selector)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "StyleSelector( " << selector << " ) [\n";
        for (const auto& prop : properties) {
            ss << prop->ToString(indent + 2) << "\n";
        }
        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    std::string selector;
    CSSPropertyList properties;
};

using StyleSelectorPtr = std::shared_ptr<StyleSelectorNode>;
using StyleSelectorList = std::vector<StyleSelectorPtr>;

} // namespace CHTL
