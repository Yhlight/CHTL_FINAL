#include "Generator.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath> // For fmod, pow

Generator::Generator(const NodeList& ast) : ast(ast) {}

// --- Expression Generation & Evaluation ---
std::string Generator::generateExpression(const std::unique_ptr<ExprNode>& expr, ElementNode* context) {
    if (!expr) return "";

    if (auto* literal = dynamic_cast<LiteralNode*>(expr.get())) {
        if (literal->value.type == TokenType::TOKEN_IDENTIFIER) {
            for (const auto& prop : context->style_properties) {
                if (prop.first == literal->value.lexeme) {
                    return generateExpression(prop.second, context);
                }
            }
        }
        std::string lexeme = literal->value.lexeme;
        if(literal->value.type == TokenType::TOKEN_STRING) {
            return lexeme.substr(1, lexeme.length() - 2);
        }
        return lexeme;
    }
    else if (auto* unary = dynamic_cast<UnaryOpNode*>(expr.get())) {
        return unary->op.lexeme + generateExpression(unary->right, context);
    }
    else if (auto* binary = dynamic_cast<BinaryOpNode*>(expr.get())) {
        std::string leftStr = generateExpression(binary->left, context);
        std::string rightStr = generateExpression(binary->right, context);
        try {
            double leftVal = std::stod(leftStr);
            double rightVal = std::stod(rightStr);
            double result = 0.0;
            switch (binary->op.type) {
                case TokenType::TOKEN_PLUS: result = leftVal + rightVal; break;
                case TokenType::TOKEN_MINUS: result = leftVal - rightVal; break;
                case TokenType::TOKEN_STAR: result = leftVal * rightVal; break;
                case TokenType::TOKEN_SLASH: result = leftVal / rightVal; break;
                case TokenType::TOKEN_PERCENT: result = fmod(leftVal, rightVal); break;
                case TokenType::TOKEN_STAR_STAR: result = pow(leftVal, rightVal); break;
                default: return leftStr + " " + binary->op.lexeme + " " + rightStr; // Not an arithmetic op
            }
            std::string str = std::to_string(result);
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            str.erase(str.find_last_not_of('.') + 1, std::string::npos);
            return str;
        } catch(...) {
            return leftStr + " " + binary->op.lexeme + " " + rightStr;
        }
    }
    else if (auto* grouping = dynamic_cast<GroupingNode*>(expr.get())) {
        return "(" + generateExpression(grouping->expression, context) + ")";
    }
    else if (auto* member = dynamic_cast<MemberAccessNode*>(expr.get())) {
        std::string selector = generateExpression(member->object, context);
        if (symbol_map.count(selector)) {
            ElementNode* targetElement = symbol_map[selector];
            for (const auto& prop : targetElement->style_properties) {
                if (prop.first == member->property.lexeme) {
                    return generateExpression(prop.second, targetElement);
                }
            }
        }
        return "/* CHTL: Ref not found */";
    }
    else if (auto* ternary = dynamic_cast<TernaryNode*>(expr.get())) {
        if (auto* condition_binary = dynamic_cast<BinaryOpNode*>(ternary->condition.get())) {
            double left_val = std::stod(generateExpression(condition_binary->left, context));
            double right_val = std::stod(generateExpression(condition_binary->right, context));

            bool condition_met = false;
            switch (condition_binary->op.type) {
                case TokenType::TOKEN_GREATER: condition_met = left_val > right_val; break;
                case TokenType::TOKEN_LESS: condition_met = left_val < right_val; break;
                case TokenType::TOKEN_EQUAL_EQUAL: condition_met = left_val == right_val; break;
                case TokenType::TOKEN_BANG_EQUAL: condition_met = left_val != right_val; break;
                case TokenType::TOKEN_GREATER_EQUAL: condition_met = left_val >= right_val; break;
                case TokenType::TOKEN_LESS_EQUAL: condition_met = left_val <= right_val; break;
                default: break;
            }

            if (condition_met) {
                return generateExpression(ternary->then_branch, context);
            } else {
                return generateExpression(ternary->else_branch, context);
            }
        }
         return "/* CHTL: Invalid ternary condition */";
    }
    return "";
}

// --- Pre-pass and Setup ---
void Generator::buildSymbolMap(const AstNodePtr& node) {
    if (!node) return;
    if (auto* element = dynamic_cast<ElementNode*>(node.get())) {
        for (const auto& attr : element->simple_attributes) {
            if (attr.first == "id") {
                symbol_map["#" + attr.second] = element;
            } else if (attr.first == "class") {
                std::stringstream ss(attr.second);
                std::string className;
                while (ss >> className) {
                    if (symbol_map.find("." + className) == symbol_map.end()) {
                        symbol_map["." + className] = element;
                    }
                }
            }
        }
        for (const auto& child : element->children) {
            buildSymbolMap(child);
        }
    }
}

// --- Main Generation ---
std::string Generator::generate() {
    for (const auto& node : ast) {
        buildSymbolMap(node);
    }
    for (const auto& node : ast) {
        generateNode(node);
    }
    return output.str();
}

void Generator::generateNode(const AstNodePtr& node) {
    if (!node) return;
    if (auto* element = dynamic_cast<ElementNode*>(node.get())) {
        visitElementNode(element);
    } else if (auto* text = dynamic_cast<TextNode*>(node.get())) {
        visitTextNode(text);
    } else if (auto* comment = dynamic_cast<CommentNode*>(node.get())) {
        visitCommentNode(comment);
    } else if (auto* origin = dynamic_cast<OriginNode*>(node.get())) {
        visitOriginNode(origin);
    } else if (auto* ns = dynamic_cast<NamespaceNode*>(node.get())) {
        visitNamespaceNode(ns);
    }
}

void Generator::visitNamespaceNode(NamespaceNode* node) {
    for (const auto& child : node->children) {
        generateNode(child);
    }
}

void Generator::visitOriginNode(OriginNode* node) {
    output << node->content;
}

void Generator::visitElementNode(ElementNode* node) {
    output << "<" << node->tagName;
    for (const auto& attr : node->simple_attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }
    if (!node->style_properties.empty()) {
        output << " style=\"";
        std::string inlineStyleString;
        for (const auto& prop : node->style_properties) {
            inlineStyleString += prop.first + ": " + generateExpression(prop.second, node) + "; ";
        }
        output << inlineStyleString.substr(0, inlineStyleString.length() - 1);
        output << "\"";
    }
    output << ">";
    for (const auto& child : node->children) {
        generateNode(child);
    }
    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(TextNode* node) {
    output << node->content;
}

void Generator::visitCommentNode(CommentNode* node) {
    output << "<!-- " << node->content << " -->";
}
