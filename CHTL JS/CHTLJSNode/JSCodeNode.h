#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL_JS {

class JSCodeNode : public Node {
public:
    JSCodeNode(std::string content) : content(std::move(content)) {}

    std::string ToString() const override {
        return "JSCode(" + content.substr(0, 20) + "...)";
    }

    std::string content;
};

} // namespace CHTL_JS
