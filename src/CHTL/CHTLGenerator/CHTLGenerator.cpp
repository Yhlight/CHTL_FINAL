#include "CHTLGenerator.h"
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace CHTL {

// Represents a CSS value with a numeric part and a unit.
struct Value {
    double Dvalue;
    std::string Svalue;
};

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
        if (a.Svalue != b.Svalue) {
            throw std::runtime_error("Unit mismatch in operation: " + a.Svalue + " and " + b.Svalue);
        }
        val.Svalue = a.Svalue;
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
        default:
            throw std::runtime_error("Unknown operator.");
    }
    return val;
}

// Evaluates a CSS expression from a vector of tokens.
std::string evaluateCssExpression(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return "";
    }

    std::stack<Value> values;
    std::stack<Token> ops;
    bool expect_operand = true;

    size_t i = 0;
    while (i < tokens.size()) {
        const auto& token = tokens[i];

        if (token.type == TokenType::Number) {
            double val = std::stod(token.lexeme);
            std::string unit = "";
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Identifier) {
                unit = tokens[i+1].lexeme;
                i++; // Consume the unit token
            }
             else if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Percent) {
                unit = "%";
                i++;
            }
            values.push({val, unit});
            expect_operand = false;
        } else if (token.type == TokenType::Minus && expect_operand) {
            // Unary minus
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Number) {
                const auto& next_token = tokens[i+1];
                double val = -std::stod(next_token.lexeme);
                std::string unit = "";
                i++; // consume number token
                if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Identifier) {
                    unit = tokens[i+1].lexeme;
                    i++; // consume unit token
                }
                 else if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Percent) {
                    unit = "%";
                    i++;
                }
                values.push({val, unit});
                expect_operand = false;
            }
        }
        else if (token.type == TokenType::Plus || token.type == TokenType::Minus ||
                   token.type == TokenType::Asterisk || token.type == TokenType::Slash ||
                   token.type == TokenType::Percent || token.type == TokenType::DoubleAsterisk) {
            while (!ops.empty() && getPrecedence(ops.top().type) >= getPrecedence(token.type)) {
                Token op = ops.top();
                ops.pop();

                if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough values for an operator.");
                Value val2 = values.top();
                values.pop();
                Value val1 = values.top();
                values.pop();

                values.push(applyOp(op.type, val2, val1));
            }
            ops.push(token);
            expect_operand = true;
        } else {
             if (values.empty() && ops.empty()) {
                if (tokens.size() == 1) return token.lexeme;
            }
        }
        i++;
    }

    while (!ops.empty()) {
        Token op = ops.top();
        ops.pop();

        if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough values for an operator.");
        Value val2 = values.top();
        values.pop();
        Value val1 = values.top();
        values.pop();

        values.push(applyOp(op.type, val2, val1));
    }

    if (values.size() != 1) {
        std::stringstream ss;
        for(const auto& token : tokens) {
            ss << token.lexeme << " ";
        }
        return ss.str();
    }

    Value result = values.top();
    std::stringstream result_ss;
    result_ss << result.Dvalue << result.Svalue;
    return result_ss.str();
}


// Helper to render a property value, which might be a simple literal or a complex expression.
std::string renderCssValue(const std::vector<Token>& tokens) {
    try {
        return evaluateCssExpression(tokens);
    } catch (const std::runtime_error& e) {
        // In case of an error (e.g., unit mismatch), fallback to calc().
        // This provides a graceful fallback and helps in debugging.
        std::stringstream ss;
        ss << "calc(";
        for (size_t i = 0; i < tokens.size(); ++i) {
            ss << tokens[i].lexeme;
            if (i < tokens.size() - 1) {
                ss << " ";
            }
        }
        ss << ")";
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
