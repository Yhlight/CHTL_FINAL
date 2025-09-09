#pragma once

#include "../CHTLNode/AST.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator : public Visitor {
public:
    std::string generate(const DocumentNode& doc);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleBlockNode& node) override;

private:
    std::stringstream m_out;
};

} // namespace CHTL
