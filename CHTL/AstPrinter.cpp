#include "AstPrinter.h"

std::string AstPrinter::print(BaseNode& node) {
    // Reset buffer for each top-level print call
    buffer.str("");
    buffer.clear();
    node.accept(*this);
    return buffer.str();
}

void AstPrinter::visit(ProgramNode& node) {
    buffer << "(Program";
    for (const auto& child : node.children) {
        buffer << "\n  ";
        child->accept(*this);
    }
    buffer << "\n)";
}

void AstPrinter::visit(ElementNode& node) {
    buffer << "(Element " << node.tagName.lexeme;
    for (const auto& attr : node.attributes) {
        buffer << "\n    ";
        attr->accept(*this);
    }
    if (node.styleBlock) {
        buffer << "\n    ";
        node.styleBlock->accept(*this);
    }
    for (const auto& child : node.children) {
        buffer << "\n    ";
        child->accept(*this);
    }
    buffer << "\n  )";
}

void AstPrinter::visit(AttributeNode& node) {
    buffer << "(Attribute " << node.name.lexeme << " ";
    if (node.value) {
        node.value->accept(*this);
    } else {
        buffer << "null";
    }
    buffer << ")";
}

void AstPrinter::visit(TextNode& node) {
    buffer << "(Text ";
    if (node.value) {
        node.value->accept(*this);
    } else {
        buffer << "null";
    }
    buffer << ")";
}

void AstPrinter::visit(CommentNode& node) {
    std::string comment_text = node.comment.lexeme;
    // Clean up the comment for printing (# and leading space)
    if (!comment_text.empty() && comment_text[0] == '#') {
        comment_text = comment_text.substr(1);
        size_t first = comment_text.find_first_not_of(" \t");
        if (std::string::npos != first) {
            comment_text = comment_text.substr(first);
        }
    }
    buffer << "(Comment \"" << comment_text << "\")";
}

void AstPrinter::visit(StringLiteralNode& node) {
    // Print the string value without the surrounding quotes for clarity
    std::string value = node.value.lexeme;
    if (value.length() >= 2) {
        buffer << "\"" << value.substr(1, value.length() - 2) << "\"";
    } else {
        buffer << value; // Should not happen with valid strings
    }
}

void AstPrinter::visit(NumberLiteralNode& node) {
    buffer << node.value.lexeme;
}

// Dummy implementations for now
void AstPrinter::visit(StyleBlockNode& node) { buffer << "(StyleBlock ...)"; }
void AstPrinter::visit(StylePropertyNode& node) { buffer << "(StyleProperty ...)"; }
void AstPrinter::visit(StyleRuleNode& node) { buffer << "(StyleRule ...)"; }
void AstPrinter::visit(TemplateStyleNode& node) { buffer << "(TemplateStyle ...)"; }
void AstPrinter::visit(TemplateElementNode& node) { buffer << "(TemplateElement ...)"; }
void AstPrinter::visit(UseStyleNode& node) { buffer << "(UseStyle ...)"; }
void AstPrinter::visit(UseElementNode& node) { buffer << "(UseElement ...)"; }

void AstPrinter::visit(UnquotedLiteralNode& node) {
    buffer << "(Unquoted";
    for (const auto& token : node.tokens) {
        buffer << " " << token.lexeme;
    }
    buffer << ")";
}
