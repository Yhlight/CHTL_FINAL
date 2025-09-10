#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/ExprNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Forward declaration for the recursive print function
void printAst(const AstNodePtr& node, int indent = 0);

void printNodeList(const NodeList& nodes, int indent) {
    for (const auto& node : nodes) {
        printAst(node, indent);
    }
}

// Forward declaration for expression printing
void printExprNode(const std::unique_ptr<ExprNode>& expr, int indent);

void printAst(const AstNodePtr& node, int indent) {
    if (!node) return;

    std::string indentation(indent * 2, ' ');

    if (auto* element = dynamic_cast<ElementNode*>(node.get())) {
        std::cout << indentation << "Element: <" << element->tagName << ">" << std::endl;
        for (const auto& attr : element->simple_attributes) {
            std::cout << indentation << "  Attribute: " << attr.first << " = \"" << attr.second << "\"" << std::endl;
        }
        for (const auto& prop : element->style_properties) {
            std::cout << indentation << "  Style Property: " << prop.first << ":" << std::endl;
            printExprNode(prop.second, indent + 2);
        }
        printNodeList(element->children, indent + 1);
    } else if (auto* text = dynamic_cast<TextNode*>(node.get())) {
        std::cout << indentation << "Text: \"" << text->content << "\"" << std::endl;
    } else if (auto* comment = dynamic_cast<CommentNode*>(node.get())) {
        std::cout << indentation << "Comment: \"" << comment->content << "\"" << std::endl;
    } else if (auto* origin = dynamic_cast<OriginNode*>(node.get())) {
        std::cout << indentation << "Origin (" << origin->type << "): \"" << origin->content << "...\"" << std::endl;
    } else if (auto* ns = dynamic_cast<NamespaceNode*>(node.get())) {
        std::cout << indentation << "Namespace: " << ns->name << std::endl;
        printNodeList(ns->children, indent + 1);
    } else if (auto* import = dynamic_cast<ImportNode*>(node.get())) {
        std::cout << indentation << "Import: @" << import->info.import_type << " from \"" << import->info.path << "\"" << std::endl;
    } else if (auto* tmpl = dynamic_cast<TemplateNode*>(node.get())) {
        std::cout << indentation << "TemplateDef: " << tmpl->name << " (" << (int)tmpl->template_type << ")" << std::endl;
        if (tmpl->template_type == TemplateType::VAR) {
            for (const auto& var : tmpl->variables) {
                std::cout << indentation << "  Var: " << var.first << " = " << var.second << std::endl;
            }
        } else if (tmpl->template_type == TemplateType::STYLE) {
            for (const auto& prop : tmpl->style_properties) {
                std::cout << indentation << "  Style Property: " << prop.first << ":" << std::endl;
                printExprNode(prop.second, indent + 2);
            }
        }
    } else if (auto* custom = dynamic_cast<CustomNode*>(node.get())) {
        std::cout << indentation << "CustomDef: " << custom->name << std::endl;
        for (const auto& prop : custom->valueless_style_properties) {
            std::cout << indentation << "  Valueless Prop: " << prop << std::endl;
        }
        for (const auto& prop : custom->style_properties) {
            std::cout << indentation << "  Style Property: " << prop.first << ":" << std::endl;
            printExprNode(prop.second, indent + 2);
        }
    } else if (auto* usage = dynamic_cast<TemplateUsageNode*>(node.get())) {
        std::cout << indentation << "TemplateUsage: " << usage->name << std::endl;
        for (const auto& prop : usage->property_overrides) {
            std::cout << indentation << "  Override Prop: " << prop.first << ":" << std::endl;
            printExprNode(prop.second, indent + 2);
        }
        for (const auto& del : usage->deleted_properties) {
            std::cout << indentation << "  Delete Prop: " << del << std::endl;
        }
    } else {
        std::cout << indentation << "Unknown Node" << std::endl;
    }
}

void printExprNode(const std::unique_ptr<ExprNode>& expr, int indent) {
    if (!expr) return;
    std::string indentation(indent * 2, ' ');

    if (auto* literal = dynamic_cast<LiteralNode*>(expr.get())) {
        std::cout << indentation << "Literal: " << literal->value.lexeme << std::endl;
    } else if (auto* unary = dynamic_cast<UnaryOpNode*>(expr.get())) {
        std::cout << indentation << "Unary Op: " << unary->op.lexeme << std::endl;
        printExprNode(unary->right, indent + 1);
    } else if (auto* binary = dynamic_cast<BinaryOpNode*>(expr.get())) {
        std::cout << indentation << "Binary Op: " << binary->op.lexeme << std::endl;
        printExprNode(binary->left, indent + 1);
        printExprNode(binary->right, indent + 1);
    } else if (auto* grouping = dynamic_cast<GroupingNode*>(expr.get())) {
        std::cout << indentation << "Grouping:" << std::endl;
        printExprNode(grouping->expression, indent + 1);
    } else if (auto* member = dynamic_cast<MemberAccessNode*>(expr.get())) {
        std::cout << indentation << "Member Access: " << member->property.lexeme << std::endl;
        printExprNode(member->object, indent + 1);
    } else if (auto* ternary = dynamic_cast<TernaryNode*>(expr.get())) {
        std::cout << indentation << "Ternary Op: ?" << std::endl;
        std::cout << indentation << "  Condition:" << std::endl;
        printExprNode(ternary->condition, indent + 2);
        std::cout << indentation << "  Then:" << std::endl;
        printExprNode(ternary->then_branch, indent + 2);
        std::cout << indentation << "  Else:" << std::endl;
        printExprNode(ternary->else_branch, indent + 2);
    } else if (auto* var = dynamic_cast<VarAccessNode*>(expr.get())) {
        std::cout << indentation << "Var Access: " << var->group_name.lexeme << "(" << var->var_name.lexeme << ")" << std::endl;
    }
}


int main() {
    std::ifstream file("Test/sample.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open Test/sample.chtl" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Starting Parser Test ---" << std::endl;

    try {
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList ast = parser.parse();

        std::cout << "--- Abstract Syntax Tree ---" << std::endl;
        printNodeList(ast, 0);

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--- Parser Test Finished ---" << std::endl;

    return 0;
}
