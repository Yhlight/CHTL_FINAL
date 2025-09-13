#include "CHTLGenerator.h"
#include <stdexcept>
#include <cmath>
#include <string_view>
#include <algorithm>

namespace CHTL {

// --- Helper Functions for Expression Evaluation ---
struct NumericValue { double value; std::string unit; };

NumericValue parseNumericValue(std::string_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(" \t"), s.size()));
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t") - 1, s.size()));
    size_t first_char = s.find_first_of("abcdefghijklmnopqrstuvwxyz%");
    if (first_char == std::string_view::npos) return {std::stod(std::string(s)), ""};
    return {std::stod(std::string(s.substr(0, first_char))), std::string(s.substr(first_char))};
}

NumericValue applyOp(TokenType op, NumericValue left, NumericValue right) {
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Unit mismatch: " + left.unit + " and " + right.unit);
    }
    std::string result_unit = left.unit.empty() ? right.unit : left.unit;
    double result_val = 0;
    switch (op) {
        case TokenType::Plus: result_val = left.value + right.value; break;
        case TokenType::Minus: result_val = left.value - right.value; break;
        case TokenType::Asterisk: result_val = left.value * right.value; break;
        case TokenType::Slash: result_val = left.value / right.value; break;
        case TokenType::Percent: result_val = fmod(left.value, right.value); break;
        case TokenType::DoubleAsterisk: result_val = pow(left.value, right.value); break;
        default: throw std::runtime_error("Unknown binary operator.");
    }
    return {result_val, result_unit};
}

std::string formatNumericValue(NumericValue val) {
    std::string s = std::to_string(val.value);
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s + val.unit;
}

// --- CHTLGenerator Main Logic ---

std::string CHTLGenerator::generate(RootNode& root) {
    // The original generator was a complex two-pass system.
    // For now, we will use a simpler single-pass rendering system.
    // This means forward references to properties might not work, but it will
    // get the basic rendering working.
    render(&root);

    std::string html = output_.str();
    std::string styles = global_styles_.str();
    if (!styles.empty()) {
        std::string style_tag = "\n    <style>\n" + styles + "    </style>\n";
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) html.insert(head_pos, style_tag);
        else html = style_tag + html;
    }
    return html;
}

void CHTLGenerator::render(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Root:
            for (const auto& child : static_cast<const RootNode*>(node)->children_) render(child.get());
            break;
        case NodeType::Element:
            renderElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            renderText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            renderComment(static_cast<const CommentNode*>(node));
            break;
        default:
            break;
    }
}

void CHTLGenerator::renderElement(const ElementNode* node) {
    indent();
    output_ << "<" << node->tagName_;
    for (const auto& attr : node->attributes_) {
        output_ << " " << attr->key_ << "=\"" << attr->value_ << "\"";
    }

    const StyleBlockNode* style_block = nullptr;
    for (const auto& child : node->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            style_block = static_cast<const StyleBlockNode*>(child.get());
            break;
        }
    }

    if (style_block) {
        output_ << " style=\"";
        for (const auto& prop : style_block->inline_properties_) {
            ExpressionEvaluator evaluator(*this, node);
            std::string value = evaluator.evaluate(*prop.value);
            output_ << prop.name << ": " << value << "; ";
        }
        output_ << "\"";

        for (const auto& rule : style_block->rules_) {
            global_styles_ << "  " << rule->selector_ << " {\n";
            for (const auto& prop : rule->properties_) {
                 ExpressionEvaluator evaluator(*this, node);
                 std::string value = evaluator.evaluate(*prop.value);
                 global_styles_ << "    " << prop.name << ": " << value << ";\n";
            }
            global_styles_ << "  }\n";
        }
    }

    bool has_children = std::any_of(node->children_.begin(), node->children_.end(), [](const auto& child){
        return child->getType() != NodeType::StyleBlock;
    });

    if (has_children) {
        output_ << ">\n";
        indentLevel_++;
        for (const auto& child : node->children_) render(child.get());
        indentLevel_--;
        indent();
        output_ << "</" << node->tagName_ << ">\n";
    } else {
        output_ << " />\n";
    }
}

void CHTLGenerator::renderText(const TextNode* node) {
    indent();
    output_ << node->text_ << "\n";
}

void CHTLGenerator::renderComment(const CommentNode* node) {
    indent();
    output_ << "<!-- " << node->comment_ << " -->\n";
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) output_ << "  ";
}

// --- ExpressionEvaluator Implementation ---

CHTLGenerator::ExpressionEvaluator::ExpressionEvaluator(CHTLGenerator& generator, const ElementNode* context_element)
    : generator_(generator), context_element_(context_element) {}

std::string CHTLGenerator::ExpressionEvaluator::evaluate(const ExpressionNode& expr) {
    expr.accept(*this);
    return result_;
}

void CHTLGenerator::ExpressionEvaluator::visit(const LiteralExpression& expr) {
    result_ = expr.value.lexeme;
}

void CHTLGenerator::ExpressionEvaluator::visit(const ReferenceExpression& expr) {
    // This simplified generator does not support property references yet.
    result_ = "/* unresolved reference */";
}

void CHTLGenerator::ExpressionEvaluator::visit(const BinaryExpression& expr) {
    std::string left_str = ExpressionEvaluator(generator_, context_element_).evaluate(*expr.left);
    std::string right_str = ExpressionEvaluator(generator_, context_element_).evaluate(*expr.right);
    try {
        NumericValue left_val = parseNumericValue(left_str);
        NumericValue right_val = parseNumericValue(right_str);
        result_ = formatNumericValue(applyOp(expr.op.type, left_val, right_val));
    } catch (const std::exception&) {
        result_ = "calc(" + left_str + " " + expr.op.lexeme + " " + right_str + ")";
    }
}

void CHTLGenerator::ExpressionEvaluator::visit(const ConditionalExpression& expr) {
    result_ = "/* conditional expression */";
}

} // namespace CHTL
