#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <memory>

namespace CHTL {

std::string Generator::generate(const NodeList& ast) {
    output_stream_.str(""); // Clear the stream
    for (const auto& node : ast) {
        if(node) {
            generateNode(node);
        }
    }
    return output_stream_.str();
}

void Generator::generateNode(const NodePtr& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        generateElement(*element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        generateText(*text);
    }
    // Other node types would be handled here in the future.
}

void Generator::generateElement(const ElementNode& node) {
    output_stream_ << getIndent() << "<" << node.tagName << ">" << "\n";
    indent_level_++;

    for (const auto& child : node.children) {
        generateNode(child);
    }

    indent_level_--;
    output_stream_ << getIndent() << "</" << node.tagName << ">" << "\n";
}

void Generator::generateText(const TextNode& node) {
    output_stream_ << getIndent() << node.content << "\n";
}

std::string Generator::getIndent() {
    return std::string(indent_level_ * 2, ' '); // 2 spaces for indentation
}

} // namespace CHTL
