#include "CHTLGenerator.h"
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace CHTL {

Value evaluateCssExpressionToValue(const std::vector<PropertyValue>& parts);
std::string evaluateCssExpression(const std::vector<PropertyValue>& parts);
ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements);

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

Value evaluateCssExpressionToValue(const std::vector<PropertyValue>& parts) {
    if (parts.empty()) {
        throw std::runtime_error("Cannot evaluate empty expression.");
    }

    std::vector<Token> tokens;
    for(const auto& part : parts) {
        if(std::holds_alternative<Token>(part)) {
            tokens.push_back(std::get<Token>(part));
        } else {
            throw std::runtime_error("Cannot evaluate property with unresolved reference.");
        }
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
             if (values.size() != 1) {
                // This is not an expression, just a literal like 'red'
                throw std::runtime_error("Expression cannot be evaluated to a single value.");
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
        throw std::runtime_error("Invalid expression.");
    }

    return values.top();
}

std::string evaluateCssExpression(const std::vector<PropertyValue>& parts) {
    try {
        Value result = evaluateCssExpressionToValue(parts);
        std::stringstream ss;
        ss << result.Dvalue << result.Svalue;
        return ss.str();
    } catch (const std::runtime_error& e) {
        std::stringstream ss;
        for (const auto& part : parts) {
            if(std::holds_alternative<Token>(part)) {
                ss << std::get<Token>(part).lexeme;
            } else {
                const auto& ref_node = std::get<PropertyReferenceNode>(part);
                ss << ref_node.selector_.lexeme << "." << ref_node.property_.lexeme;
            }
        }
        return ss.str();
    }
}

ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements) {
    if (selector.empty()) return nullptr;

    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        for (auto* elem : elements) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "id" && attr->value_ == id) {
                    return elem;
                }
            }
        }
    } else if (selector[0] == '.') {
        std::string className = selector.substr(1);
        for (auto* elem : elements) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "class") {
                    if (attr->value_ == className) {
                        return elem;
                    }
                }
            }
        }
    } else {
        for (auto* elem : elements) {
            if (elem->tagName_ == selector) {
                return elem;
            }
        }
    }

    return nullptr;
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
                bool has_reference = false;
                for (const auto& part : prop.second) {
                    if (std::holds_alternative<PropertyReferenceNode>(part)) {
                        has_reference = true;
                        break;
                    }
                }

                if (has_reference) {
                    unresolved_properties_.push_back({node, prop.first, prop.second});
                } else {
                    node->computed_styles_[prop.first] = evaluateCssExpression(prop.second);
                    if (!element_id.empty()) {
                        try {
                           symbol_table_[element_id][prop.first] = evaluateCssExpressionToValue(prop.second);
                        } catch (const std::runtime_error& e) {
                        }
                    }
                }
            }
        }
        firstPass(child.get());
    }
}

void CHTLGenerator::secondPass() {
    for (auto& unresolved : unresolved_properties_) {
        std::vector<PropertyValue> resolved_parts;
        for (const auto& part : unresolved.value_parts) {
            if (std::holds_alternative<PropertyReferenceNode>(part)) {
                const auto& ref_node = std::get<PropertyReferenceNode>(part);
                ElementNode* target_element = findElementBySelector(ref_node.selector_.lexeme, all_elements_);

                if (target_element) {
                    std::string unique_id = getElementUniqueId(target_element);
                    if(symbol_table_.count(unique_id) && symbol_table_.at(unique_id).count(ref_node.property_.lexeme)) {
                        Value resolved_value = symbol_table_.at(unique_id).at(ref_node.property_.lexeme);
                        resolved_parts.emplace_back(Token{TokenType::Number, std::to_string(resolved_value.Dvalue), 0, 0});
                        if (!resolved_value.Svalue.empty()) {
                            if (resolved_value.Svalue == "%") {
                                 resolved_parts.emplace_back(Token{TokenType::Percent, "%", 0, 0});
                            } else {
                                 resolved_parts.emplace_back(Token{TokenType::Identifier, resolved_value.Svalue, 0, 0});
                            }
                        }
                    } else {
                        throw std::runtime_error("Could not resolve property reference: " + ref_node.selector_.lexeme + "." + ref_node.property_.lexeme);
                    }
                }
                else {
                    throw std::runtime_error("Could not find element for selector: " + ref_node.selector_.lexeme);
                }
            } else {
                resolved_parts.push_back(part);
            }
        }
        unresolved.element->computed_styles_[unresolved.property_name] = evaluateCssExpression(resolved_parts);
    }
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
