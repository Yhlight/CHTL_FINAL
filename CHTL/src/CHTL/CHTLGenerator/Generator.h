#pragma once

#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>

namespace CHTL {

// The Generator class is responsible for traversing the Abstract Syntax Tree (AST)
// and producing the final output code, which for now is HTML with CSS.
class Generator {
public:
    Generator() = default;

    // The main entry point for code generation.
    std::string generate(const DocumentNode* document);

private:
    void generateNode(const BaseNode* node);
    void generateElement(const ElementNode* element);
    void generateText(const TextNode* text);

    // The output string for the main HTML content.
    std::string m_output;
    // The output string for collecting global CSS rules.
    std::string m_global_css;
};

} // namespace CHTL
