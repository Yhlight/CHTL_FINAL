#pragma once

#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>

namespace CHTL {

// The Generator class is responsible for traversing the Abstract Syntax Tree (AST)
// and producing the final output code, which for now is HTML.
class Generator {
public:
    Generator() = default;

    // The main entry point for code generation.
    // It takes the root of the AST and returns the generated HTML string.
    std::string generate(const DocumentNode* document);

private:
    // Recursive helper functions to generate code for specific node types.
    void generateNode(const BaseNode* node);
    void generateElement(const ElementNode* element);
    void generateText(const TextNode* text);

    // The output string is built up during the generation process.
    std::string m_output;
};

} // namespace CHTL
