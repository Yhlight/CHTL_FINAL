#include "CHTLGenerator.h"
#include <map>
#include <stack>
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace CHTL {

// --- CSS Value Calculation Utilities ---

struct CssValue {
    double value;
    std::string unit;

    std::string toString() const {
        std::stringstream ss;
        ss << value;
        ss << unit;
        return ss.str();
    }
};

// Parses a string like "100px" into a CssValue {100.0, "px"}
CssValue parseCssValue(const std::string& s) {
    if (s.empty()) {
        throw std::invalid_argument("Cannot parse empty string.");
    }

    size_t i = 0;
    if (s[0] == '-' || s[0] == '+') {
        i = 1;
    }
    while (i < s.length() && (std::isdigit(s[i]) || s[i] == '.')) {
        i++;
    }

    std::string num_part = s.substr(0, i);
    std::string unit_part = s.substr(i);

    if (num_part.empty()) {
         throw std::invalid_argument("No numeric part in value: " + s);
    }

    return {std::stod(num_part), unit_part};
}

// Applies an operator to two CssValue operands
CssValue applyOp(const Token& op, const CssValue& b, const CssValue& a) {
    switch (op.type) {
        case TokenType::Plus:
            if (a.unit != b.unit) throw std::runtime_error("Mismatched units for +: '" + a.unit + "' and '" + b.unit + "'");
            return {a.value + b.value, a.unit};
        case TokenType::Minus:
            if (a.unit != b.unit) throw std::runtime_error("Mismatched units for -: '" + a.unit + "' and '" + b.unit + "'");
            return {a.value - b.value, a.unit};
        case TokenType::Asterisk:
            if (a.unit.empty()) return {a.value * b.value, b.unit};
            if (b.unit.empty()) return {a.value * b.value, a.unit};
            throw std::runtime_error("One operand for * must be unitless.");
        case TokenType::Slash:
            if (b.value == 0) throw std::runtime_error("Division by zero.");
            if (b.unit.empty()) return {a.value / b.value, a.unit};
            // Dividing units results in a unitless number, e.g. 100px / 10px = 10
            if (a.unit == b.unit) return {a.value / b.value, ""};
            throw std::runtime_error("Mismatched units for /.");
        case TokenType::Percent:
             if (b.value == 0) throw std::runtime_error("Modulo by zero.");
             if (a.unit != b.unit) throw std::runtime_error("Mismatched units for %: '" + a.unit + "' and '" + b.unit + "'");
             return {std::fmod(a.value, b.value), a.unit};
        case TokenType::DoubleAsterisk:
            if (!b.unit.empty()) throw std::runtime_error("Exponent for ** must be unitless.");
            return {std::pow(a.value, b.value), a.unit};
        default:
            throw std::runtime_error("Unknown operator: " + op.lexeme);
    }
}

// --- End of CSS Value Calculation Utilities ---

// Function to get operator precedence
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
            return 0; // Not an operator
    }
}

// Evaluates a CSS expression token stream into a single value using Shunting-yard algorithm
CssValue evaluateCssExpression(const std::vector<Token>& tokens) {
    std::stack<CssValue> values;
    std::stack<Token> ops;

    auto process_ops = [&]() {
        if (ops.size() < 1 || values.size() < 2) throw std::runtime_error("Invalid expression syntax for operator processing.");
        Token op = ops.top();
        ops.pop();

        CssValue val2 = values.top();
        values.pop();

        CssValue val1 = values.top();
        values.pop();

        values.push(applyOp(op, val2, val1));
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        if (token.type == TokenType::Number) {
            std::string value_str = token.lexeme;
            std::string unit_str = "";
            // Check if next token is a unit identifier
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Identifier) {
                unit_str = tokens[i+1].lexeme;
                i++; // Consume the unit token
            }
            values.push({std::stod(value_str), unit_str});
        } else if (token.type == TokenType::OpenParen) {
            ops.push(token);
        } else if (token.type == TokenType::CloseParen) {
            while (!ops.empty() && ops.top().type != TokenType::OpenParen) {
                process_ops();
            }
            if (ops.empty()) throw std::runtime_error("Mismatched parentheses in expression.");
            ops.pop(); // Pop the left parenthesis
        } else { // Is an operator
            while (!ops.empty() && ops.top().type != TokenType::OpenParen &&
                   (getPrecedence(ops.top().type) > getPrecedence(token.type) ||
                    (getPrecedence(ops.top().type) == getPrecedence(token.type) && token.type != TokenType::DoubleAsterisk))) {
                process_ops();
            }
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        if (ops.top().type == TokenType::OpenParen) throw std::runtime_error("Mismatched parentheses in expression.");
        process_ops();
    }

    if (values.size() != 1 || !ops.empty()) {
        throw std::runtime_error("The expression is malformed or could not be fully evaluated.");
    }

    return values.top();
}


// Helper to render a property value. If it's a complex expression, it is evaluated.
std::string renderCssValue(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return "";
    }

    bool has_operator = false;
    for (const auto& token : tokens) {
        if (getPrecedence(token.type) > 0) {
            has_operator = true;
            break;
        }
    }

    if (has_operator) {
        try {
            CssValue result = evaluateCssExpression(tokens);
            return result.toString();
        } catch (const std::exception& e) {
            // In a real compiler, you'd have more sophisticated error reporting.
            // For now, we'll rethrow to halt compilation with a message.
            throw std::runtime_error("Failed to evaluate CSS expression: " + std::string(e.what()));
        }
    } else {
        // No operators, just concatenate tokens (e.g., "1px", "solid", "black" or just "red")
        std::stringstream ss;
        for (const auto& token : tokens) {
            ss << token.lexeme;
        }
        return ss.str();
    }
}


std::string CHTLGenerator::generate(const RootNode& root) {
    for (const auto& child : root.children_) {
        visit(child.get());
    }

    std::string html = output_.str();
    std::string styles = global_styles_.str();

    if (!styles.empty()) {
        std::string style_tag = "\n    <style>\n" + styles + "    </style>\n";
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, style_tag);
        } else {
            // If no head, maybe prepend to body or just wrap everything?
            // For now, we'll just prepend it to the whole document if no head is found.
            html = style_tag + html;
        }
    }

    return html;
}

void CHTLGenerator::visit(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element:
            visitElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            visitText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            visitComment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::Origin:
            visitOrigin(static_cast<const OriginNode*>(node));
            break;
        case NodeType::StyleBlock:
            // Style blocks are handled by their parent element, so we do nothing here.
            break;
        default:
            // Handle other node types or throw an error
            break;
    }
}

void CHTLGenerator::visitElement(const ElementNode* node) {
    // --- Attribute and Style Processing Pass ---
    std::map<std::string, std::string> attributes;
    // Copy existing attributes
    for (const auto& attr : node->attributes_) {
        attributes[attr->key_] = attr->value_;
    }

    std::stringstream inline_style_ss;
    for (const auto& child : node->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            const auto* styleNode = static_cast<const StyleBlockNode*>(child.get());
            // Process inline properties
            for (const auto& prop : styleNode->inline_properties_) {
                inline_style_ss << prop.first << ": " << renderCssValue(prop.second) << "; ";
            }
            // Process rules
            for (const auto& rule : styleNode->rules_) {
                std::string final_selector = rule->selector_;
                // Resolve '&' context selector
                if (rule->selector_[0] == '&') {
                    std::string context_selector;
                    if (attributes.count("id")) {
                        context_selector = "#" + attributes["id"];
                    } else if (attributes.count("class")) {
                        // Use the first class name as the context
                        context_selector = "." + attributes["class"].substr(0, attributes["class"].find(" "));
                    }
                    if (!context_selector.empty()) {
                        final_selector.replace(0, 1, context_selector);
                    }
                }

                // 1. Add rule to global styles
                global_styles_ << "      " << final_selector << " {\n";
                for (const auto& prop : rule->properties_) {
                    global_styles_ << "        " << prop.first << ": " << renderCssValue(prop.second) << ";\n";
                }
                global_styles_ << "      }\n";

                // 2. Inject class/id attribute
                if (rule->selector_[0] == '.') {
                    std::string className = rule->selector_.substr(1);
                    if (attributes.count("class")) {
                        // Prevent adding duplicate class names
                        if ((" " + attributes["class"] + " ").find(" " + className + " ") == std::string::npos) {
                            attributes["class"] += " " + className;
                        }
                    } else {
                        attributes["class"] = className;
                    }
                } else if (rule->selector_[0] == '#') {
                    attributes["id"] = rule->selector_.substr(1);
                }
            }
        }
    }

    std::string inline_style_attr = inline_style_ss.str();
    if (!inline_style_attr.empty()) {
        attributes["style"] = inline_style_attr;
    }

    // --- Rendering Pass ---
    indent();
    output_ << "<" << node->tagName_;
    for(const auto& attr_pair : attributes) {
        output_ << " " << attr_pair.first << "=\"" << attr_pair.second << "\"";
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
            visit(child.get());
        }
        indentLevel_--;
        indent();
        output_ << "</" << node->tagName_ << ">\n";
    }
}

void CHTLGenerator::visitText(const TextNode* node) {
    indent();
    output_ << node->text_ << "\n";
}

void CHTLGenerator::visitComment(const CommentNode* node) {
    indent();
    // CHTL generator comments '--' are converted to HTML comments
    output_ << "<!-- " << node->comment_ << " -->\n";
}

void CHTLGenerator::visitOrigin(const OriginNode* node) {
    // Simply append the raw content, no processing or indentation.
    output_ << node->content_;
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "  "; // 2 spaces for indentation
    }
}

} // namespace CHTL
