#include "CHTLGenerator.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/LiteralExpressionNode.h"
#include "CHTLNode/InfixExpressionNode.h"
#include "CHTLNode/ConditionalExpressionNode.h"
#include "CHTLObject/NumberObject.h"
#include "CHTLObject/StringObject.h"
#include "CHTLObject/BooleanObject.h"
#include <stdexcept>
#include <vector>

namespace CHTL {

// Helper to parse a string like "100px" into a NumberObject
ObjectPtr parseDimension(const std::string& literal) {
    size_t i = 0;
    while (i < literal.length() && (std::isdigit(literal[i]) || literal[i] == '.')) {
        i++;
    }
    try {
        double val = std::stod(literal.substr(0, i));
        std::string unit = literal.substr(i);
        return std::make_shared<NumberObject>(val, unit);
    } catch (const std::invalid_argument& e) {
        // Not a number, treat as a string
        return std::make_shared<StringObject>(literal);
    }
}

CHTLGenerator::CHTLGenerator(NodePtr rootNode) : m_rootNode(std::move(rootNode)) {}

std::string CHTLGenerator::Generate() {
    if (m_rootNode) {
        visit(m_rootNode);
    }
    std::string final_output = m_output.str();
    std::string global_styles = m_global_css.str();
    if (!global_styles.empty()) {
        std::string style_tag = "<style>" + global_styles + "</style>";
        size_t head_pos = final_output.find("</head>");
        if (head_pos != std::string::npos) {
            final_output.insert(head_pos, style_tag);
        } else {
            final_output = style_tag + final_output;
        }
    }
    return final_output;
}

ObjectPtr CHTLGenerator::evaluateInfixExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right) {
    if (left->Type() == ObjectType::NUMBER_OBJ && right->Type() == ObjectType::NUMBER_OBJ) {
        auto leftVal = std::static_pointer_cast<NumberObject>(left);
        auto rightVal = std::static_pointer_cast<NumberObject>(right);
        if (op == "+") return std::make_shared<NumberObject>(leftVal->value + rightVal->value, leftVal->unit);
        if (op == "-") return std::make_shared<NumberObject>(leftVal->value - rightVal->value, leftVal->unit);
        if (op == "*") return std::make_shared<NumberObject>(leftVal->value * rightVal->value, leftVal->unit);
        if (op == "/") return std::make_shared<NumberObject>(leftVal->value / rightVal->value, leftVal->unit);
        if (op == ">") return std::make_shared<BooleanObject>(leftVal->value > rightVal->value);
        if (op == "<") return std::make_shared<BooleanObject>(leftVal->value < rightVal->value);
    }
    // Fallback for other types or errors
    return std::make_shared<StringObject>("ERROR: Invalid infix operation");
}

ObjectPtr CHTLGenerator::evaluate(const ExpressionPtr& node, EvalContext& context) {
    if (auto literal = std::dynamic_pointer_cast<LiteralExpressionNode>(node)) {
        // Check if it's a variable in the context
        if (context.count(literal->token.literal)) {
            return context.at(literal->token.literal);
        }
        // Otherwise, parse it
        return parseDimension(literal->token.literal);
    }
    if (auto infix = std::dynamic_pointer_cast<InfixExpressionNode>(node)) {
        ObjectPtr left = evaluate(infix->left, context);
        ObjectPtr right = evaluate(infix->right, context);
        return evaluateInfixExpression(infix->op.literal, left, right);
    }
    if (auto conditional = std::dynamic_pointer_cast<ConditionalExpressionNode>(node)) {
        ObjectPtr condition_res = evaluate(conditional->condition, context);
        if (auto bool_obj = std::dynamic_pointer_cast<BooleanObject>(condition_res)) {
            if (bool_obj->value) {
                return evaluate(conditional->consequence, context);
            } else {
                return evaluate(conditional->alternative, context);
            }
        }
    }
    return std::make_shared<StringObject>("ERROR: Cannot evaluate expression");
}

std::string CHTLGenerator::generateExpression(const ExpressionPtr& node, bool is_sub_expression) {
    if (auto literal = std::dynamic_pointer_cast<LiteralExpressionNode>(node)) {
        return literal->token.literal;
    }
    if (auto infix = std::dynamic_pointer_cast<InfixExpressionNode>(node)) {
        std::string left = generateExpression(infix->left, true);
        std::string right = generateExpression(infix->right, true);
        std::string result = left + " " + infix->op.literal + " " + right;
        if (is_sub_expression) return "(" + result + ")";
        return "calc(" + result + ")";
    }
    return "INVALID_EXPRESSION";
}

void CHTLGenerator::visit(const NodePtr& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitText(text);
    } else if (auto style = std::dynamic_pointer_cast<StyleNode>(node)) {}
}

void CHTLGenerator::visitElement(const std::shared_ptr<ElementNode>& node) {
    m_output << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        m_output << " ";
        visitAttribute(attr);
    }

    CSSPropertyList all_inline_properties;
    NodeList other_children;
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            all_inline_properties.insert(all_inline_properties.end(), styleNode->inline_properties.begin(), styleNode->inline_properties.end());
            for (const auto& selector : styleNode->selector_blocks) {
                 m_global_css << selector->selector << " {";
                for (const auto& prop : selector->properties) {
                    m_global_css << prop->key << ":" << generateExpression(prop->value) << ";";
                }
                m_global_css << "}";
            }
        } else {
            other_children.push_back(child);
        }
    }

    if (!all_inline_properties.empty()) {
        EvalContext context;
        std::vector<CSSPropertyPtr> deferred_properties;
        // Pass 1: Evaluate simple properties and populate context
        for (const auto& prop : all_inline_properties) {
            if (std::dynamic_pointer_cast<ConditionalExpressionNode>(prop->value)) {
                deferred_properties.push_back(prop);
            } else {
                context[prop->key] = evaluate(prop->value, context);
            }
        }
        // Pass 2: Evaluate conditional (deferred) properties
        for (const auto& prop : deferred_properties) {
            context[prop->key] = evaluate(prop->value, context);
        }

        m_output << " style=\"";
        for (const auto& pair : context) {
            m_output << pair.first << ":" << pair.second->RawValue() << ";";
        }
        m_output << "\"";
    }

    m_output << ">";
    for (const auto& child : other_children) {
        visit(child);
    }
    m_output << "</" << node->tagName << ">";
}

void CHTLGenerator::visitText(const std::shared_ptr<TextNode>& node) {
    m_output << node->text;
}

void CHTLGenerator::visitAttribute(const std::shared_ptr<AttributeNode>& node) {
    m_output << node->key << "=\"" << node->value << "\"";
}

}
