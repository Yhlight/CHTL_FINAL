#include "CHTLGenerator.h"
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "CHTL/CHTLNode/ExpressionNode.h"

namespace CHTL {

// Forward declarations
Value evaluate(const std::vector<PropertyValue>& parts, CHTLGenerator* generator);
Value applyOp(TokenType op, Value b, Value a);

struct ExpressionEvaluator {
    CHTLGenerator* generator;

    Value operator()(const Token& t) const {
        if (t.type == TokenType::Number) {
            return {std::stod(t.lexeme), "", ""};
        }
        return {0.0, "", t.lexeme};
    }

    Value operator()(const PropertyReferenceNode& prn) const {
        // This requires access to the generator's state, which is tricky.
        // For now, this is a simplified stub.
        return {0.0, "", "0px"};
    }

    Value operator()(const std::unique_ptr<ArithmeticNode>& an) const {
        Value left = evaluate(an->left, generator);
        Value right = evaluate(an->right, generator);
        return applyOp(an->op.type, right, left);
    }

    Value operator()(const std::unique_ptr<ConditionalNode>& cn) const {
        Value cond_val = evaluate(cn->condition, generator);
        // A simple truthiness check. CHTL.md doesn't specify details.
        if (cond_val.Dvalue != 0 || !cond_val.stringValue.empty()) {
            return evaluate(cn->true_branch, generator);
        } else {
            return evaluate(cn->false_branch, generator);
        }
    }
};

Value evaluate(const std::vector<PropertyValue>& parts, CHTLGenerator* generator) {
    if (parts.empty()) {
        throw std::runtime_error("Cannot evaluate empty expression.");
    }
    // For now, we assume expressions are not mixed with plain values
    // and that an expression is always the first and only part.
    return std::visit(ExpressionEvaluator{generator}, parts[0]);
}


// Returns the precedence of an operator.
int getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus:
            return 1;
        case TokenType::Asterisk:
        case TokenType::Slash:
        case TokenType::Percent:
            return 2;
        case TokenType::DoubleAsterisk:
            return 3;
        default:
            return 0;
    }
}

// Applies an operator to two values.
Value applyOp(TokenType op, Value b, Value a) {
    Value val;

    if (op == TokenType::Asterisk || op == TokenType::Slash) {
        if (op == TokenType::Slash && !a.Svalue.empty() && a.Svalue == b.Svalue) {
            val.Svalue = ""; // Units cancel out
        } else if (a.Svalue.empty() && !b.Svalue.empty()) {
            val.Svalue = b.Svalue;
        } else if (!a.Svalue.empty() && b.Svalue.empty()) {
            val.Svalue = a.Svalue;
        } else if (a.Svalue.empty() && b.Svalue.empty()) {
            val.Svalue = "";
        } else {
            throw std::runtime_error("Invalid unit operation for '*' or '/'.");
        }
    } else { // For +, -, %, **
        if (a.Svalue != b.Svalue && !a.Svalue.empty() && !b.Svalue.empty()) {
            throw std::runtime_error("Unit mismatch in operation: " + a.Svalue + " and " + b.Svalue);
        }
        val.Svalue = a.Svalue.empty() ? b.Svalue : a.Svalue;
    }

    switch (op) {
        case TokenType::Plus:
            val.Dvalue = a.Dvalue + b.Dvalue;
            break;
        case TokenType::Minus:
            val.Dvalue = a.Dvalue - b.Dvalue;
            break;
        case TokenType::Asterisk:
            val.Dvalue = a.Dvalue * b.Dvalue;
            break;
        case TokenType::Slash:
            if (b.Dvalue == 0) throw std::runtime_error("Division by zero.");
            val.Dvalue = a.Dvalue / b.Dvalue;
            break;
        case TokenType::Percent:
            val.Dvalue = fmod(a.Dvalue, b.Dvalue);
            break;
        case TokenType::DoubleAsterisk:
            val.Dvalue = pow(a.Dvalue, b.Dvalue);
            break;
        // For logical/comparison
        case TokenType::GreaterThan:
            val.Dvalue = a.Dvalue > b.Dvalue;
            break;
        case TokenType::LessThan:
            val.Dvalue = a.Dvalue < b.Dvalue;
            break;
        case TokenType::DoubleAmpersand:
            val.Dvalue = (a.Dvalue != 0) && (b.Dvalue != 0);
            break;
        case TokenType::DoublePipe:
            val.Dvalue = (a.Dvalue != 0) || (b.Dvalue != 0);
            break;
        default:
            throw std::runtime_error("Unknown operator.");
    }
    return val;
}

std::string CHTLGenerator::generate(RootNode& root) {
    for (auto& child : root.children_) {
        firstPass(child.get());
    }

    secondPass();

    for (const auto& child : root.children_) {
        render(child.get());
    }

    std::string html = output_.str();
    std::string styles = global_styles_.str();

    if (!styles.empty()) {
        std::string style_tag = "\n    <style>\n" + styles + "    </style>\n";
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, style_tag);
        } else {
            html = style_tag + html;
        }
    }

    return html;
}

void CHTLGenerator::firstPass(Node* node) {
    if (!node) return;
    if (node->getType() == NodeType::Element) {
        firstPassVisitElement(static_cast<ElementNode*>(node));
    }
}

void CHTLGenerator::firstPassVisitElement(ElementNode* node) {
    all_elements_.push_back(node);
    std::string element_id = getElementUniqueId(node);

    for (auto& child : node->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            auto* styleNode = static_cast<StyleBlockNode*>(child.get());
            for (const auto& prop : styleNode->inline_properties_) {
                // Simplified logic: just evaluate and store.
                // A full implementation needs to handle unresolved references.
                Value value = evaluate(prop.second, this);
                 std::stringstream ss;
                if (!value.stringValue.empty()) {
                    ss << value.stringValue;
                } else {
                    ss << value.Dvalue << value.Svalue;
                }
                node->computed_styles_[prop.first] = ss.str();
                if (!element_id.empty()) {
                   symbol_table_[element_id][prop.first] = value;
                }
            }
        }
        firstPass(child.get());
    }
}

void CHTLGenerator::secondPass() {
    // This logic needs to be updated to handle expression re-evaluation
    // For now, it's disabled to allow compilation.
}

void CHTLGenerator::render(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element:
            renderElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            renderText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            renderComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Origin:
            renderOrigin(static_cast<const OriginNode*>(node));
            break;
        case NodeType::StyleBlock:
            break;
        default:
            break;
    }
}

void CHTLGenerator::renderElement(const ElementNode* node) {
    indent();
    output_ << "<" << node->tagName_;

    for(const auto& attr_pair : node->attributes_) {
        output_ << " " << attr_pair->key_ << "=\"" << attr_pair->value_ << "\"";
    }

    if (!node->computed_styles_.empty()) {
        output_ << " style=\"";
        for (const auto& style : node->computed_styles_) {
            output_ << style.first << ": " << style.second << "; ";
        }
        output_ << "\"";
    }

    bool hasNonStyleChildren = false;
    for (const auto& child : node->children_) {
        if (child->getType() != NodeType::StyleBlock) {
            hasNonStyleChildren = true;
            break;
        }
    }

    if (!hasNonStyleChildren) {
        output_ << " />\n";
    } else {
        output_ << ">\n";
        indentLevel_++;
        for (const auto& child : node->children_) {
            render(child.get());
        }
        indentLevel_--;
        indent();
        output_ << "</" << node->tagName_ << ">\n";
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

void CHTLGenerator::renderOrigin(const OriginNode* node) {
    output_ << node->content_;
}

std::string CHTLGenerator::getElementUniqueId(const ElementNode* node) {
    for (const auto& attr : node->attributes_) {
        if (attr->key_ == "id") {
            return "#" + attr->value_;
        }
    }
    for (const auto& attr : node->attributes_) {
        if (attr->key_ == "class") {
            return "." + attr->value_;
        }
    }
    return "";
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "  ";
    }
}

} // namespace CHTL
