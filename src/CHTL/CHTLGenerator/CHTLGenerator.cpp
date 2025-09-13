#include "CHTLGenerator.h"
#include "../CHTLLexer/CHTLLexer.h" // For re-tokenizing
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace CHTL {

// Forward declarations for our new structured approach
Value evaluateArithmetic(const std::vector<PropertyValue>& parts);
std::string formatValue(const Value& val);
Value resolvePropertyValue(const std::vector<PropertyValue>& parts);
ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements);

// --- Core Arithmetic Evaluation Logic ---

int getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::Plus: case TokenType::Minus: return 1;
        case TokenType::Asterisk: case TokenType::Slash: case TokenType::Percent: return 2;
        case TokenType::DoubleAsterisk: return 3;
        default: return 0;
    }
}

Value applyOp(TokenType op, Value b, Value a) {
    Value val;
    if (op == TokenType::Asterisk || op == TokenType::Slash) {
        if (op == TokenType::Slash && !a.Svalue.empty() && a.Svalue == b.Svalue) val.Svalue = "";
        else if (a.Svalue.empty() && !b.Svalue.empty()) val.Svalue = b.Svalue;
        else if (!a.Svalue.empty() && b.Svalue.empty()) val.Svalue = a.Svalue;
        else if (a.Svalue.empty() && b.Svalue.empty()) val.Svalue = "";
        else throw std::runtime_error("Invalid unit operation for '*' or '/'.");
    } else {
        if (a.Svalue != b.Svalue && !a.Svalue.empty() && !b.Svalue.empty()) {
             throw std::runtime_error("Unit mismatch in operation: " + a.Svalue + " and " + b.Svalue);
        }
        val.Svalue = a.Svalue.empty() ? b.Svalue : a.Svalue;
    }

    switch (op) {
        case TokenType::Plus: val.Dvalue = a.Dvalue + b.Dvalue; break;
        case TokenType::Minus: val.Dvalue = a.Dvalue - b.Dvalue; break;
        case TokenType::Asterisk: val.Dvalue = a.Dvalue * b.Dvalue; break;
        case TokenType::Slash: if (b.Dvalue == 0) throw std::runtime_error("Division by zero."); val.Dvalue = a.Dvalue / b.Dvalue; break;
        case TokenType::Percent: val.Dvalue = fmod(a.Dvalue, b.Dvalue); break;
        case TokenType::DoubleAsterisk: val.Dvalue = pow(a.Dvalue, b.Dvalue); break;
        default: throw std::runtime_error("Unknown operator.");
    }
    return val;
}

Value evaluateArithmetic(const std::vector<PropertyValue>& parts) {
    std::vector<Token> tokens;
    bool has_plus_or_minus = false;
    std::string first_unit = "";
    bool has_mixed_units_for_calc = false;

    for(const auto& part : parts) {
        if(std::holds_alternative<Token>(part)) {
            const auto& token = std::get<Token>(part);
            tokens.push_back(token);
            if (token.type == TokenType::Plus || token.type == TokenType::Minus) {
                has_plus_or_minus = true;
            }
            // A simple heuristic to find units: check for identifiers or percent sign
            // that follow a number. This is imperfect but covers many cases.
            if (token.type == TokenType::Identifier || token.type == TokenType::Percent) {
                if (first_unit.empty()) {
                    first_unit = token.lexeme;
                } else if (first_unit != token.lexeme) {
                    has_mixed_units_for_calc = true;
                }
            }
        } else {
            throw std::runtime_error("Cannot evaluate property with unresolved reference during arithmetic.");
        }
    }

    // If we have mixed units with + or -, fallback to a calc() expression
    if (has_plus_or_minus && has_mixed_units_for_calc) {
        std::stringstream ss;
        ss << "calc(";
        for (size_t i = 0; i < tokens.size(); ++i) {
            ss << tokens[i].lexeme;
            // CSS calc() requires spaces around + and -
            if (i < tokens.size() - 1 && (tokens[i].type == TokenType::Plus || tokens[i].type == TokenType::Minus || tokens[i+1].type == TokenType::Plus || tokens[i+1].type == TokenType::Minus)) {
                ss << " ";
            }
        }
        ss << ")";
        return {0, "", ss.str()};
    }

    // --- Original Shunting-Yard Implementation ---
    std::stack<Value> values;
    std::stack<Token> ops;
    bool expect_operand = true;
    size_t i = 0;

    while (i < tokens.size()) {
        const auto& token = tokens[i];
        if (token.type == TokenType::Number) {
            double val = std::stod(token.lexeme);
            std::string unit = "";
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Identifier) { unit = tokens[++i].lexeme; }
            else if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Percent) { unit = "%"; i++; }
            values.push({val, unit, ""});
            expect_operand = false;
        } else if (token.type == TokenType::Minus && expect_operand) {
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Number) {
                double val = -std::stod(tokens[++i].lexeme);
                std::string unit = "";
                if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Identifier) { unit = tokens[++i].lexeme; }
                else if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::Percent) { unit = "%"; i++;}
                values.push({val, unit, ""});
                expect_operand = false;
            }
        } else if (getPrecedence(token.type) > 0) {
            while (!ops.empty() && getPrecedence(ops.top().type) >= getPrecedence(token.type)) {
                Token op = ops.top(); ops.pop();
                if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough values for an operator.");
                Value val2 = values.top(); values.pop();
                Value val1 = values.top(); values.pop();
                values.push(applyOp(op.type, val2, val1));
            }
            ops.push(token);
            expect_operand = true;
        } else {
             throw std::runtime_error("Invalid token in arithmetic expression: " + token.lexeme);
        }
        i++;
    }

    while (!ops.empty()) {
        Token op = ops.top(); ops.pop();
        if (values.size() < 2) throw std::runtime_error("Invalid expression: not enough values for an operator.");
        Value val2 = values.top(); values.pop();
        Value val1 = values.top(); values.pop();
        values.push(applyOp(op.type, val2, val1));
    }

    if (values.size() != 1) throw std::runtime_error("Invalid expression.");
    return values.top();
}


// --- New Top-Level Resolution and Formatting Logic ---

std::string formatValue(const Value& val) {
    if (!val.stringValue.empty()) {
        return val.stringValue;
    }
    std::stringstream ss;
    ss << val.Dvalue;
    std::string s = ss.str();
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (!s.empty() && s.back() == '.') {
            s.pop_back();
        }
    }
    return s + val.Svalue;
}

Value resolvePropertyValue(const std::vector<PropertyValue>& parts) {
    if (parts.empty()) {
        return {0, "", ""};
    }

    bool is_expression = false;
    for (const auto& part : parts) {
        if (std::holds_alternative<Token>(part)) {
            if (getPrecedence(std::get<Token>(part).type) > 0) {
                is_expression = true;
                break;
            }
        }
    }

    if (is_expression) {
        return evaluateArithmetic(parts);
    } else {
        std::stringstream ss;
        bool first = true;
        for (const auto& part : parts) {
            if (!first) ss << " ";
            if(std::holds_alternative<Token>(part)) {
                ss << std::get<Token>(part).lexeme;
            } else {
                const auto& ref_node = std::get<PropertyReferenceNode>(part);
                ss << ref_node.selector_.lexeme << "." << ref_node.property_.lexeme;
            }
            first = false;
        }
        return {0, "", ss.str()};
    }
}


// --- Selector and Generator Logic ---

ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements) {
    if (selector.empty()) return nullptr;

    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        for (auto* elem : elements) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "id" && attr->value_ == id) return elem;
            }
        }
    } else if (selector[0] == '.') {
        std::string className = selector.substr(1);
        for (auto* elem : elements) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "class") {
                    if (attr->value_.find(className) != std::string::npos) return elem;
                }
            }
        }
    } else {
        for (auto* elem : elements) { if (elem->tagName_ == selector) return elem; }
        for (auto* elem : elements) { for (const auto& attr : elem->attributes_) { if (attr->key_ == "id" && attr->value_ == selector) return elem; } }
        for (auto* elem : elements) { for (const auto& attr : elem->attributes_) { if (attr->key_ == "class" && attr->value_.find(selector) != std::string::npos) return elem; } }
    }
    return nullptr;
}


std::string CHTLGenerator::generate(RootNode& root) {
    for (auto& child : root.children_) firstPass(child.get());
    secondPass();
    for (const auto& child : root.children_) render(child.get());

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
                    Value value = resolvePropertyValue(prop.second);
                    node->computed_styles_[prop.first] = formatValue(value);
                    if (!element_id.empty()) {
                       symbol_table_[element_id][prop.first] = value;
                    }
                }
            }

            for (const auto& rule : styleNode->rules_) {
                const std::string& selector = rule->selector_;
                if (selector.length() > 1 && selector[0] == '.') {
                    std::string className = selector.substr(1);
                    AttributeNode* classAttr = nullptr;
                    for (auto& attr : node->attributes_) {
                        if (attr->key_ == "class") { classAttr = attr.get(); break; }
                    }
                    if (classAttr) {
                        if (classAttr->value_.find(className) == std::string::npos) classAttr->value_ += " " + className;
                    } else {
                        node->attributes_.push_back(std::make_unique<AttributeNode>("class", className));
                    }
                } else if (selector.length() > 1 && selector[0] == '#') {
                    std::string idName = selector.substr(1);
                    bool idExists = false;
                    for (const auto& attr : node->attributes_) { if (attr->key_ == "id") { idExists = true; break; } }
                    if (!idExists) node->attributes_.push_back(std::make_unique<AttributeNode>("id", idName));
                }
            }

            std::string element_selector;
            for (const auto& attr : node->attributes_) {
                if (attr->key_ == "class") {
                    element_selector = "." + attr->value_.substr(0, attr->value_.find(' '));
                    break;
                }
            }
            if (element_selector.empty()) {
                for (const auto& attr : node->attributes_) {
                    if (attr->key_ == "id") { element_selector = "#" + attr->value_; break; }
                }
            }

            for (const auto& rule : styleNode->rules_) {
                std::string final_selector = rule->selector_;
                if (final_selector.find('&') != std::string::npos) {
                    if (element_selector.empty()) throw std::runtime_error("Cannot resolve '&' selector: parent element has no class or id for context.");
                    size_t pos = 0;
                    while ((pos = final_selector.find('&', pos)) != std::string::npos) {
                        final_selector.replace(pos, 1, element_selector);
                        pos += element_selector.length();
                    }
                }
                global_styles_ << "  " << final_selector << " {\n";
                for (const auto& prop : rule->properties_) {
                    Value value = resolvePropertyValue(prop.second);
                    global_styles_ << "    " << prop.first << ": " << formatValue(value) << ";\n";
                }
                global_styles_ << "  }\n";
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
                if (!target_element) throw std::runtime_error("Could not find element for selector: " + ref_node.selector_.lexeme);

                std::string unique_id = getElementUniqueId(target_element);
                if(symbol_table_.count(unique_id) && symbol_table_.at(unique_id).count(ref_node.property_.lexeme)) {
                    Value resolved_value = symbol_table_.at(unique_id).at(ref_node.property_.lexeme);
                    if (!resolved_value.stringValue.empty()) {
                        CHTLLexer temp_lexer(resolved_value.stringValue);
                        std::vector<Token> retokenized_parts = temp_lexer.scanTokens();
                        if (!retokenized_parts.empty() && retokenized_parts.back().type == TokenType::EndOfFile) {
                            retokenized_parts.pop_back();
                        }
                        for (const auto& token : retokenized_parts) {
                            resolved_parts.emplace_back(token);
                        }
                    } else {
                        resolved_parts.emplace_back(Token{TokenType::Number, std::to_string(resolved_value.Dvalue), 0, 0});
                        if (!resolved_value.Svalue.empty()) {
                             if (resolved_value.Svalue == "%") {
                                resolved_parts.emplace_back(Token{TokenType::Percent, "%", 0, 0});
                             } else {
                                resolved_parts.emplace_back(Token{TokenType::Identifier, resolved_value.Svalue, 0, 0});
                             }
                        }
                    }
                } else {
                    throw std::runtime_error("Could not resolve property reference: " + ref_node.selector_.lexeme + "." + ref_node.property_.lexeme);
                }
            } else {
                resolved_parts.push_back(part);
            }
        }
        Value final_value = resolvePropertyValue(resolved_parts);
        unresolved.element->computed_styles_[unresolved.property_name] = formatValue(final_value);
    }
}

void CHTLGenerator::render(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element: renderElement(static_cast<const ElementNode*>(node)); break;
        case NodeType::Text: renderText(static_cast<const TextNode*>(node)); break;
        case NodeType::Comment: renderComment(static_cast<const CommentNode*>(node)); break;
        case NodeType::Origin: renderOrigin(static_cast<const OriginNode*>(node)); break;
        case NodeType::StyleBlock: break; // StyleBlocks are processed, not rendered directly
        default: break;
    }
}

void CHTLGenerator::renderElement(const ElementNode* node) {
    indent();
    output_ << "<" << node->tagName_;
    for(const auto& attr_pair : node->attributes_) { output_ << " " << attr_pair->key_ << "=\"" << attr_pair->value_ << "\""; }
    if (!node->computed_styles_.empty()) {
        output_ << " style=\"";
        for (const auto& style : node->computed_styles_) {
            output_ << style.first << ": " << style.second << "; ";
        }
        output_ << "\"";
    }

    bool hasNonStyleChildren = false;
    for (const auto& child : node->children_) { if (child->getType() != NodeType::StyleBlock) { hasNonStyleChildren = true; break; } }

    if (!hasNonStyleChildren) {
        output_ << " />\n";
    } else {
        output_ << ">\n";
        indentLevel_++;
        for (const auto& child : node->children_) { render(child.get()); }
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
    for (const auto& attr : node->attributes_) { if (attr->key_ == "id") return "#" + attr->value_; }
    for (const auto& attr : node->attributes_) { if (attr->key_ == "class") return "." + attr->value_.substr(0, attr->value_.find(' ')); }
    return "";
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) output_ << "  ";
}

} // namespace CHTL
