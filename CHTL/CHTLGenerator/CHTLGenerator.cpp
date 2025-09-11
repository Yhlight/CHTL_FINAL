#include "CHTLGenerator.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <cmath>
#include <algorithm>

namespace CHTL {

// --- Helper Functions ---

std::string ValueToString(const EvaluatedValue& val) {
    if (val.type == ValueType::String) {
        return std::get<std::string>(val.value);
    }
    if (val.type == ValueType::Number) {
        std::stringstream ss;
        ss << std::get<double>(val.value);
        return ss.str() + val.unit;
    }
     if (val.type == ValueType::Bool) {
        return std::get<bool>(val.value) ? "true" : "false";
    }
    return "";
}

std::string CHTLGenerator::escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}


// --- Main Generation Logic ---

std::string CHTLGenerator::Generate(const NodeList& ast) {
    m_output_body.str("");
    m_output_css.str("");
    for (const auto& node : ast) {
        Visit(node);
    }

    std::string css = m_output_css.str();
    std::string body = m_output_body.str();

    if (css.empty()) {
        return body;
    }

    return "<style>\n" + css + "</style>\n" + body;
}

void CHTLGenerator::Visit(const NodePtr& node) {
    if (!node) return;
    switch (node->GetType()) {
        case NodeType::Element: VisitElement(static_cast<const ElementNode*>(node.get())); break;
        case NodeType::Text: VisitText(static_cast<const TextNode*>(node.get())); break;
        case NodeType::Comment: VisitComment(static_cast<const CommentNode*>(node.get())); break;
        // Style nodes are visited from within VisitElement, not at the top level
        case NodeType::Style: break;
        default: break;
    }
}

void CHTLGenerator::VisitElement(const ElementNode* node) {
    static const std::set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    m_output_body << "<" << node->GetTagName();

    // --- Attribute and Style Processing ---
    std::stringstream inline_style_stream;
    std::string primary_selector;

    // First pass for attributes to determine primary selector
    for (const auto& prop : node->GetProperties()) {
        if (prop.name == "id") {
            primary_selector = "#" + ValueToString(EvaluateExpression(prop.value, node));
        } else if (prop.name == "class" && primary_selector.empty()) {
            primary_selector = "." + ValueToString(EvaluateExpression(prop.value, node));
        }
        EvaluatedValue val = EvaluateExpression(prop.value, node);
        m_output_body << " " << prop.name << "=\"" << escapeHTML(ValueToString(val)) << "\"";
    }

    // Second pass for style blocks
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());

            // Process direct properties for inline styles
            for (const auto& prop : styleNode->GetProperties()) {
                EvaluatedValue val = EvaluateExpression(prop.value, node);
                inline_style_stream << prop.name << ":" << ValueToString(val) << ";";
            }

            // Process nested rules for hoisting
            for (const auto& rule : styleNode->GetRules()) {
                VisitStyleRule(rule.get(), node, primary_selector);
            }
        }
    }

    std::string inline_style = inline_style_stream.str();
    if (!inline_style.empty()) {
        m_output_body << " style=\"" << escapeHTML(inline_style) << "\"";
    }
    // --- End Attribute and Style Processing ---

    m_output_body << ">";

    if (selfClosingTags.count(node->GetTagName()) == 0) {
        for (const auto& child : node->GetChildren()) {
            if (child->GetType() != NodeType::Style) {
                Visit(child);
            }
        }
        m_output_body << "</" << node->GetTagName() << ">";
    }
}

void CHTLGenerator::VisitText(const TextNode* node) {
    m_output_body << escapeHTML(node->GetText());
}

void CHTLGenerator::VisitComment(const CommentNode* node) {
    if (node->GetCommentType() == CommentType::Generator) {
        m_output_body << "<!--" << escapeHTML(node->GetText()) << "-->";
    }
}

void CHTLGenerator::VisitStyleRule(const StyleRuleNode* node, const ElementNode* context, const std::string& primary_selector) {
    std::string final_selector = node->GetSelector();
    // Handle contextual '&' selector
    if (final_selector.rfind('&', 0) == 0) {
        if (primary_selector.empty()) {
            throw std::runtime_error("Cannot use '&' selector on an element with no class or id.");
        }
        final_selector.replace(0, 1, primary_selector);
    }

    m_output_css << final_selector << " {\n";
    for (const auto& prop : node->GetProperties()) {
        EvaluatedValue val = EvaluateExpression(prop.value, context);
        m_output_css << "  " << prop.name << ": " << ValueToString(val) << ";\n";
    }
    m_output_css << "}\n";
}


// --- Full Expression Evaluation ---

EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* context) {
    if (!expr) return {ValueType::String, ""};

    switch (expr->GetType()) {
        case ExpressionNodeType::StringLiteral:
            return {ValueType::String, static_cast<const StringLiteralNode*>(expr.get())->GetValue()};
        case ExpressionNodeType::NumberLiteral:
            return {ValueType::Number, static_cast<const NumberLiteralNode*>(expr.get())->GetValue(), static_cast<const NumberLiteralNode*>(expr.get())->GetUnit()};
        case ExpressionNodeType::BinaryOp:
            return VisitBinaryOp(static_cast<const BinaryOpNode*>(expr.get()), context);
        case ExpressionNodeType::Conditional:
            return VisitConditionalExpr(static_cast<const ConditionalExprNode*>(expr.get()), context);
        default:
            throw std::runtime_error("Unsupported expression type in generator.");
    }
}

EvaluatedValue CHTLGenerator::VisitBinaryOp(const BinaryOpNode* node, const ElementNode* context) {
    EvaluatedValue left = EvaluateExpression(node->GetLeft(), context);
    EvaluatedValue right = EvaluateExpression(node->GetRight(), context);
    TokenType op = node->GetOperator();

    if (op == TokenType::LogicalAnd || op == TokenType::LogicalOr) {
        bool l_bool = (left.type == ValueType::Bool) ? std::get<bool>(left.value) : (left.type == ValueType::Number && std::get<double>(left.value) != 0);
        bool r_bool = (right.type == ValueType::Bool) ? std::get<bool>(right.value) : (right.type == ValueType::Number && std::get<double>(right.value) != 0);
        return {ValueType::Bool, (op == TokenType::LogicalAnd ? (l_bool && r_bool) : (l_bool || r_bool))};
    }

    if (op == TokenType::Plus && (left.type == ValueType::String || right.type == ValueType::String)) {
        return {ValueType::String, ValueToString(left) + ValueToString(right)};
    }

    if (left.type != ValueType::Number || right.type != ValueType::Number) {
        throw std::runtime_error("Arithmetic and comparison operators require numeric operands.");
    }

    double l_val = std::get<double>(left.value);
    double r_val = std::get<double>(right.value);

    if (op >= TokenType::DoubleEquals && op <= TokenType::LessThanOrEquals) {
        bool result = false;
        switch (op) {
            case TokenType::DoubleEquals: result = (l_val == r_val); break;
            case TokenType::NotEquals: result = (l_val != r_val); break;
            case TokenType::GreaterThan: result = (l_val > r_val); break;
            case TokenType::GreaterThanOrEquals: result = (l_val >= r_val); break;
            case TokenType::LessThan: result = (l_val < r_val); break;
            case TokenType::LessThanOrEquals: result = (l_val <= r_val); break;
            default: break;
        }
        return {ValueType::Bool, result};
    }

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Cannot perform arithmetic on numbers with incompatible units.");
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_val = 0;
    switch (op) {
        case TokenType::Plus: result_val = l_val + r_val; break;
        case TokenType::Minus: result_val = l_val - r_val; break;
        case TokenType::Star: result_val = l_val * r_val; break;
        case TokenType::Slash: if (r_val == 0) throw std::runtime_error("Division by zero."); result_val = l_val / r_val; break;
        case TokenType::Percent: result_val = fmod(l_val, r_val); break;
        case TokenType::DoubleStar: result_val = pow(l_val, r_val); break;
        default: throw std::runtime_error("Unsupported binary operator.");
    }
    return {ValueType::Number, result_val, result_unit};
}

EvaluatedValue CHTLGenerator::VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* context) {
    EvaluatedValue condition = EvaluateExpression(node->GetCondition(), context);
    bool condition_bool = (condition.type == ValueType::Bool) ? std::get<bool>(condition.value) : (condition.type == ValueType::Number && std::get<double>(condition.value) != 0);

    if (condition_bool) {
        return EvaluateExpression(node->GetTrueBranch(), context);
    } else {
        if (node->GetFalseBranch()) {
            return EvaluateExpression(node->GetFalseBranch(), context);
        }
        return {ValueType::String, ""};
    }
}

} // namespace CHTL
