#include "Generator.h"
#include <stdexcept>
#include <cmath>
#include "../CHTLNode/StyleNode.h"

namespace CHTL
{
    Generator::Generator(std::unique_ptr<ProgramNode> ast)
        : m_ast(std::move(ast)) {}

    std::string Generator::generate() {
        if (!m_ast) return "";
        collectSymbols(m_ast.get());
        visit(m_ast.get());

        std::string final_html;
        std::string global_style_content = m_global_styles.str();

        if (!global_style_content.empty()) {
            final_html += "<style>\n";
            final_html += global_style_content;
            final_html += "</style>\n";
        }
        final_html += m_output.str();
        return final_html;
    }

    void Generator::visit(BaseNode* node) {
        if (!node) return;
        switch (node->getType()) {
            case NodeType::PROGRAM: visitProgramNode(static_cast<ProgramNode*>(node)); break;
            case NodeType::ELEMENT_NODE: visitElementNode(static_cast<ElementNode*>(node)); break;
            case NodeType::TEXT_NODE: visitTextNode(static_cast<TextNode*>(node)); break;
            case NodeType::COMMENT_NODE: visitCommentNode(static_cast<CommentNode*>(node)); break;
            case NodeType::STYLE_NODE: visitStyleNode(static_cast<StyleNode*>(node)); break;
            default: throw std::runtime_error("Unknown node type in generator");
        }
    }

    void Generator::visitProgramNode(ProgramNode* node) {
        for (const auto& child : node->children) {
            visit(child.get());
        }
    }

    void Generator::visitElementNode(ElementNode* node) {
        std::stringstream inline_style_ss;
        m_evaluation_stack.clear(); // Clear stack for each new element's style evaluation

        for (const auto& child : node->children) {
            if (child->getType() == NodeType::STYLE_NODE) {
                StyleNode* styleNode = static_cast<StyleNode*>(child.get());

                for (const auto& prop : styleNode->inline_properties) {
                    EvaluatedValue val = evaluate(prop.second.get());
                    std::cout << "  [DEBUG] Final value for " << prop.first << " is ";
                    if (val.unit == "is_string_literal") {
                        std::cout << val.stringValue;
                    } else {
                        std::cout << val.value << val.unit;
                    }
                    std::cout << std::endl;
                    inline_style_ss << prop.first << ":";
                    if (val.unit == "is_string_literal") {
                        inline_style_ss << val.stringValue;
                    } else {
                        inline_style_ss << val.value << val.unit;
                    }
                    inline_style_ss << ";";
                }

                for (const auto& rule : styleNode->rules) {
                    if (rule.selector.rfind('.', 0) == 0) {
                        std::string current_class = node->attributes.count("class") ? node->attributes["class"] + " " : "";
                        node->attributes["class"] = current_class + rule.selector.substr(1);
                    } else if (rule.selector.rfind('#', 0) == 0) {
                        node->attributes["id"] = rule.selector.substr(1);
                    }

                    std::string final_selector = rule.selector;
                    if (final_selector.find('&') != std::string::npos) {
                        std::string base_selector;
                        if (node->attributes.count("id")) base_selector = "#" + node->attributes["id"];
                        else if (node->attributes.count("class")) base_selector = "." + node->attributes["class"].substr(0, node->attributes["class"].find(' '));
                        final_selector.replace(final_selector.find('&'), 1, base_selector);
                    }

                    m_global_styles << final_selector << " {\n";
                    for (const auto& prop : rule.properties) {
                        EvaluatedValue val = evaluate(prop.second.get());
                        m_global_styles << "  " << prop.first << ": ";
                        if (val.unit == "is_string_literal") m_global_styles << val.stringValue;
                        else m_global_styles << val.value << val.unit;
                        m_global_styles << ";\n";
                    }
                    m_global_styles << "}\n";
                }
            }
        }

        m_output << std::string(m_indentLevel * 2, ' ') << "<" << node->tagName;

        std::string inline_style_str = inline_style_ss.str();
        if (!inline_style_str.empty()) {
            node->attributes["style"] = inline_style_str;
        }

        for (const auto& attr : node->attributes) {
            m_output << " " << attr.first << "=\"" << attr.second << "\"";
        }

        m_output << ">\n";

        m_indentLevel++;
        for (const auto& child : node->children) {
            if (child->getType() != NodeType::STYLE_NODE) visit(child.get());
        }
        m_indentLevel--;
        m_output << std::string(m_indentLevel * 2, ' ') << "</" << node->tagName << ">\n";
    }

    void Generator::visitTextNode(TextNode* node) {
        m_output << std::string(m_indentLevel * 2, ' ') << node->content << "\n";
    }

    void Generator::visitCommentNode(CommentNode* node) {
        if (node->commentType == ChtlCommentType::Generator) {
            m_output << std::string(m_indentLevel * 2, ' ') << "<!-- " << node->content << " -->\n";
        }
    }

    void Generator::visitStyleNode(StyleNode* node) {}

    void Generator::collectSymbols(BaseNode* node) {
        if (!node) return;
        if (node->getType() == NodeType::ELEMENT_NODE) {
            ElementNode* el = static_cast<ElementNode*>(node);
            for (const auto& child : el->children) {
                if(child->getType() == NodeType::STYLE_NODE) {
                    StyleNode* styleNode = static_cast<StyleNode*>(child.get());
                    for (const auto& rule : styleNode->rules) {
                        if (rule.selector.rfind('.', 0) == 0) m_symbol_table[rule.selector] = el;
                        else if (rule.selector.rfind('#', 0) == 0) m_symbol_table[rule.selector] = el;
                    }
                }
            }
            if (el->attributes.count("id")) m_symbol_table["#" + el->attributes["id"]] = el;
            if (el->attributes.count("class")) {
                std::stringstream ss(el->attributes["class"]);
                std::string single_class;
                while(ss >> single_class) m_symbol_table["." + single_class] = el;
            }
            for (const auto& child : el->children) collectSymbols(child.get());
        } else if (node->getType() == NodeType::PROGRAM) {
            for (const auto& child : static_cast<ProgramNode*>(node)->children) collectSymbols(child.get());
        }
    }

#include <iostream> // For debugging

    EvaluatedValue Generator::evaluate(ExprNode* node) {
        if (!node) throw std::runtime_error("Cannot evaluate null expression.");

        std::cout << "  Evaluating: " << node->toString() << " (type: " << (int)node->getType() << ")" << std::endl;

        switch (node->getType()) {
            case ExprType::NUMBER_LITERAL: {
                auto* numNode = static_cast<NumberLiteralNode*>(node);
                return {numNode->value, numNode->unit, ""};
            }
            case ExprType::STRING_LITERAL: {
                auto* strNode = static_cast<StringLiteralNode*>(node);
                return {0.0, "is_string_literal", strNode->value};
            }
            case ExprType::ATTRIBUTE_REFERENCE: {
                auto* refNode = static_cast<AttributeReferenceNode*>(node);
                return evaluateStyleProperty(refNode->selector, refNode->propertyName);
            }
            case ExprType::BINARY_OP: {
                auto* opNode = static_cast<BinaryOpNode*>(node);
                EvaluatedValue left = evaluate(opNode->left.get());
                EvaluatedValue right = evaluate(opNode->right.get());
                bool leftIsNum = left.unit != "is_string_literal";
                bool rightIsNum = right.unit != "is_string_literal";

                if (!leftIsNum || !rightIsNum) {
                    if (opNode->op == '+') return {0.0, "is_string_literal", left.stringValue + right.stringValue};
                    throw std::runtime_error("Arithmetic on non-numeric value.");
                }
                if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
                    throw std::runtime_error("Mismatched units.");
                }
                std::string unit = !left.unit.empty() ? left.unit : right.unit;
                double val = 0;
                switch (opNode->op) {
                    case '+': val = left.value + right.value; break;
                    case '-': val = left.value - right.value; break;
                    case '*': val = left.value * right.value; break;
                    case '/': if (right.value == 0) throw std::runtime_error("Division by zero."); val = left.value / right.value; break;
                    case '%': val = std::fmod(left.value, right.value); break;
                    case '^': val = std::pow(left.value, right.value); break;
                    default: throw std::runtime_error("Unknown operator.");
                }
                std::cout << "  [DEBUG] BinaryOp returning: " << val << unit << std::endl;
                return {val, unit, ""};
            }
            default: throw std::runtime_error("Unknown expression node type.");
        }
    }

    EvaluatedValue Generator::evaluateStyleProperty(const std::string& selector, const std::string& propertyName) {
        if (m_symbol_table.find(selector) == m_symbol_table.end()) {
            throw std::runtime_error("Ref error: selector '" + selector + "' not found.");
        }
        ElementNode* targetNode = m_symbol_table[selector];
        if (m_evaluation_stack.count(targetNode)) {
            throw std::runtime_error("Circular dependency on selector '" + selector + "'.");
        }
        m_evaluation_stack.insert(targetNode);

        ExprNode* propExpr = nullptr;
        for (const auto& child : targetNode->children) {
            if (child->getType() == NodeType::STYLE_NODE) {
                auto* styleNode = static_cast<StyleNode*>(child.get());
                if (styleNode->inline_properties.count(propertyName)) {
                    propExpr = styleNode->inline_properties[propertyName].get();
                    break;
                }
            }
        }
        if (!propExpr) {
            throw std::runtime_error("Ref error: property '" + propertyName + "' not found on '" + selector + "'.");
        }

        EvaluatedValue result = evaluate(propExpr);
        m_evaluation_stack.erase(targetNode);
        return result;
    }
}
