#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLContext.h"

std::string Generator::generate(std::shared_ptr<BaseNode> root, CHTLContext& context) {
    output.clear();
    indentLevel = 0;
    if (root) {
        root->accept(*this);
    }

    // Post-processing step: inject global CSS into the <head>
    if (!context.globalCssContent.empty()) {
        std::string styleBlock = "\n    <style>\n" + context.globalCssContent + "    </style>\n";
        size_t head_end_pos = output.find("</head>");
        if (head_end_pos != std::string::npos) {
            output.insert(head_end_pos, styleBlock);
        } else {
            // As a fallback if no <head> is present, just prepend it.
            // A better solution would be to ensure a <head> exists.
            output.insert(0, styleBlock);
        }
    }

    return output;
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output += "  "; // 2 spaces for indentation
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    output += "<" + node.tagName;

    // Append attributes
    for (const auto& attr : node.attributes) {
        output += " " + attr.first + "=\"" + attr.second + "\"";
    }

    // Append processed inline styles
    if (!node.processedStyles.empty()) {
        output += " style=\"";
        for (const auto& style : node.processedStyles) {
            output += style.first + ": " + style.second + ";";
        }
        output += "\"";
    }

    if (node.children.empty()) {
        // Self-closing tag for simplicity, though not all tags work this way.
        // A more robust implementation would have a list of void elements.
        output += " />\n";
    } else {
        output += ">\n";
        indentLevel++;
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        indentLevel--;
        indent();
        output += "</" + node.tagName + ">\n";
    }
}

void Generator::visit(TextNode& node) {
    indent();
    output += node.text + "\n";
}

void Generator::visit(CommentNode& node) {
    indent();
    output += "<!-- " + node.comment + " -->\n";
}

void Generator::visit(StyleNode& node) {
    // StyleNodes should be processed and removed from the AST before generation.
    // If one gets here, we simply ignore it.
}

void Generator::visit(StyleTemplateNode& node) {
    // Template definitions are not rendered in the final output.
}

void Generator::visit(ElementTemplateNode& node) {
    // Template definitions are not rendered in the final output.
}

void Generator::visit(ElementTemplateUsageNode& node) {
    // These should be replaced by the TemplateProcessor before generation.
    // If one gets here, it's an error, but we'll just ignore it.
}
