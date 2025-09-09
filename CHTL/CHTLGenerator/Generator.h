#pragma once

#include "../CHTLNode/AST.h"
#include "../CHTLContext/Context.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator : public Visitor {
public:
    explicit Generator(CHTLContext& context);
    std::string generate(const DocumentNode& doc);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleBlockNode& node) override;
    void visit(CssPropertyNode& node) override;
    void visit(StyleTemplateDefinitionNode& node) override;
    void visit(StyleTemplateUsageNode& node) override;
    void visit(ImportNode& node) override;

private:
    CHTLContext& m_context;
    std::stringstream m_out;
    bool m_is_in_style_block = false; // To control output format
};

} // namespace CHTL
