#pragma once

#include "Node.h"
#include "ElementNode.h"
#include "TextNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator(const Program& program);
    std::string Generate();

private:
    void visit(const Node* node);

    const Program& m_program;
    std::stringstream m_output;
};

} // namespace CHTL
