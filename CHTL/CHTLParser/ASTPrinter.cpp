#include "ASTPrinter.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLGenerator/ExprGenerator.h"

void ASTPrinter::print(const NodeList& nodes) {
    std::cout << "--- AST ---" << std::endl;
    for (const auto& node : nodes) {
        node->accept(*this);
    }
    std::cout << "-----------" << std::endl;
}

void ASTPrinter::visit(ElementNode* node) {
    printIndent();
    std::cout << "<" << node->tagName << ">" << std::endl;
    indent();
    for (const auto& child : node->children) {
        child->accept(*this);
    }
    dedent();
    printIndent();
    std::cout << "</" << node->tagName << ">" << std::endl;
}

void ASTPrinter::visit(TextNode* node) {
    printIndent();
    std::cout << "Text: \"" << node->text << "\"" << std::endl;
}

void ASTPrinter::visit(CommentNode* node) {
    printIndent();
    if (node->isGeneratorComment) {
        std::cout << "Generator Comment: " << node->text << std::endl;
    } else {
        std::cout << "Comment: " << node->text << std::endl;
    }
}

void ASTPrinter::visit(PropertyNode* node) {
    ExprGenerator exprGen;
    std::string valueStr = exprGen.generate(node->value.get());
    printIndent();
    std::cout << "Property: " << node->name << " = \"" << valueStr << "\"" << std::endl;
}

void ASTPrinter::visit(StyleNode* node) {
    printIndent();
    std::cout << "<style>" << std::endl;
    indent();
    for (const auto& child : node->children) {
        child->accept(*this);
    }
    dedent();
    printIndent();
    std::cout << "</style>" << std::endl;
}

void ASTPrinter::visit(StyleTemplateDefinitionNode* node) {
    printIndent();
    std::cout << "[Template] @Style " << node->name << std::endl;
    indent();
    for (const auto& prop : node->properties) {
        prop->accept(*this);
    }
    dedent();
}

void ASTPrinter::visit(ElementTemplateDefinitionNode* node) {
    printIndent();
    std::cout << "[Template] @Element " << node->name << std::endl;
    indent();
    for (const auto& child : node->children) {
        child->accept(*this);
    }
    dedent();
}

void ASTPrinter::visit(VarTemplateDefinitionNode* node) {
    printIndent();
    std::cout << "[Template] @Var " << node->name << std::endl;
    indent();
    for (const auto& var : node->variables) {
        var->accept(*this);
    }
    dedent();
}

void ASTPrinter::visit(StyleUsageNode* node) {
    printIndent();
    std::cout << "@Style " << node->name << std::endl;
}

void ASTPrinter::visit(ElementUsageNode* node) {
    printIndent();
    std::cout << "@Element " << node->name << std::endl;
}

void ASTPrinter::indent() {
    indent_level++;
}

void ASTPrinter::dedent() {
    indent_level--;
}

void ASTPrinter::printIndent() {
    for (int i = 0; i < indent_level; ++i) {
        std::cout << "  ";
    }
}
