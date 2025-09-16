#include "CHTLGenerator.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/ResponsiveValueNode.h"
#include "../CHTLNode/ScriptBlockNode.h"
#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../../CHTLJS/CHTLJSNode/SequenceNode.h"
#include "../../CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<ParserContext> context) : context_(std::move(context)) {}

std::string formatValue(const Value& val);
ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements);

int getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::DoublePipe: return 1;
        case TokenType::DoubleAmpersand: return 2;
        case TokenType::DoubleEquals: case TokenType::NotEquals: return 3;
        case TokenType::GreaterThan: case TokenType::LessThan: case TokenType::GreaterThanEquals: case TokenType::LessThanEquals: return 4;
        case TokenType::Plus: case TokenType::Minus: return 5;
        case TokenType::Asterisk: case TokenType::Slash: case TokenType::Percent: return 6;
        case TokenType::DoubleAsterisk: return 7;
        default: return 0;
    }
}

Value applyOp(TokenType op, Value b, Value a) {
    Value val;
    val.Svalue = a.Svalue.empty() ? b.Svalue : a.Svalue;
    switch (op) {
        case TokenType::Plus: val.Dvalue = a.Dvalue + b.Dvalue; break;
        case TokenType::Minus: val.Dvalue = a.Dvalue - b.Dvalue; break;
        case TokenType::Asterisk: val.Dvalue = a.Dvalue * b.Dvalue; break;
        case TokenType::Slash: if (b.Dvalue == 0) throw std::runtime_error("Division by zero."); val.Dvalue = a.Dvalue / b.Dvalue; break;
        case TokenType::Percent: val.Dvalue = fmod(a.Dvalue, b.Dvalue); break;
        case TokenType::DoubleAsterisk: val.Dvalue = pow(a.Dvalue, b.Dvalue); break;
        case TokenType::GreaterThan: val.Dvalue = a.Dvalue > b.Dvalue; val.Svalue = ""; break;
        case TokenType::LessThan: val.Dvalue = a.Dvalue < b.Dvalue; val.Svalue = ""; break;
        case TokenType::GreaterThanEquals: val.Dvalue = a.Dvalue >= b.Dvalue; val.Svalue = ""; break;
        case TokenType::LessThanEquals: val.Dvalue = a.Dvalue <= b.Dvalue; val.Svalue = ""; break;
        case TokenType::DoubleEquals: val.Dvalue = a.Dvalue == b.Dvalue; val.Svalue = ""; break;
        case TokenType::NotEquals: val.Dvalue = a.Dvalue != b.Dvalue; val.Svalue = ""; break;
        case TokenType::DoubleAmpersand: val.Dvalue = a.Dvalue && b.Dvalue; val.Svalue = ""; break;
        case TokenType::DoublePipe: val.Dvalue = a.Dvalue || b.Dvalue; val.Svalue = ""; break;
        default: throw std::runtime_error("Unknown operator.");
    }
    return val;
}

Value CHTLGenerator::evaluateExpression(std::vector<PropertyValue>::const_iterator& it, const std::vector<PropertyValue>::const_iterator& end, int min_precedence) {
    Value lhs;
    if (it == end) throw std::runtime_error("Unexpected end of expression.");
    const auto& current_part = *it;

    if (std::holds_alternative<ResponsiveValueNode>(current_part)) {
        const auto& responsive_node = std::get<ResponsiveValueNode>(current_part);
        this->responsive_variables_.insert(responsive_node.getVariableName());
        lhs = {0, "", "var(--" + responsive_node.getVariableName() + ")"};
        ++it;
    } else if (std::holds_alternative<PropertyReferenceNode>(current_part)) {
        const auto& ref_node = std::get<PropertyReferenceNode>(current_part);
        ElementNode* target_element = findElementBySelector(ref_node.selector_.lexeme, all_elements_);
        if (!target_element) {
            throw std::runtime_error("Could not find element for selector: " + ref_node.selector_.lexeme);
        }
        std::string unique_id = getElementUniqueId(target_element);
        if(symbol_table_.count(unique_id) && symbol_table_.at(unique_id).count(ref_node.property_.lexeme)) {
            lhs = symbol_table_.at(unique_id).at(ref_node.property_.lexeme);
            ++it;
        } else {
            throw std::runtime_error("Could not resolve property reference: " + ref_node.selector_.lexeme + "." + ref_node.property_.lexeme);
        }
    } else if (std::holds_alternative<Token>(current_part)) {
        const auto& token = std::get<Token>(current_part);
        if (token.type == TokenType::OpenParen) {
            ++it;
            lhs = evaluateExpression(it, end, 0);
            if (it == end || !std::holds_alternative<Token>(*it) || std::get<Token>(*it).type != TokenType::CloseParen) {
                throw std::runtime_error("Expected ')' to close expression.");
            }
            ++it;
        } else if (token.type == TokenType::Number) {
            ++it;
            double d_val = std::stod(token.lexeme);
            std::string unit;
            if (it != end && std::holds_alternative<Token>(*it) && (std::get<Token>(*it).type == TokenType::Identifier || std::get<Token>(*it).type == TokenType::Percent)) {
                unit = std::get<Token>(*it).lexeme;
                ++it;
            }
            lhs = {d_val, unit, ""};
        } else if (token.type == TokenType::StringLiteral || token.type == TokenType::UnquotedLiteral) {
            ++it;
            lhs = {0, "", token.lexeme};
        } else if (token.type == TokenType::Minus) {
            ++it;
            lhs = evaluateExpression(it, end, 10);
            lhs.Dvalue = -lhs.Dvalue;
        } else {
             throw std::runtime_error("Unexpected token in expression: " + token.lexeme);
        }
    } else {
        throw std::runtime_error("Unhandled variant type in expression evaluator.");
    }

    while (it != end) {
        if (!std::holds_alternative<Token>(*it)) break;
        TokenType op_type = std::get<Token>(*it).type;

        if (op_type == TokenType::QuestionMark) {
            const int TERNARY_PRECEDENCE = 1;
            if (TERNARY_PRECEDENCE < min_precedence) break;
            ++it;
            Value true_val = evaluateExpression(it, end, 0);
            if (it == end || !std::holds_alternative<Token>(*it) || std::get<Token>(*it).type != TokenType::Colon) {
                 throw std::runtime_error("Expected ':' for ternary operator.");
            }
            ++it;
            Value false_val = evaluateExpression(it, end, TERNARY_PRECEDENCE);
            lhs = (lhs.Dvalue != 0) ? true_val : false_val;
            continue;
        }

        int precedence = getPrecedence(op_type);
        if (precedence == 0 || precedence < min_precedence) break;
        int next_min_precedence = (op_type == TokenType::DoubleAsterisk) ? precedence : precedence + 1;
        ++it;
        Value rhs = evaluateExpression(it, end, next_min_precedence);
        lhs = applyOp(op_type, rhs, lhs);
    }
    return lhs;
}

std::string formatValue(const Value& val) {
    if (!val.stringValue.empty()) return val.stringValue;
    std::stringstream ss;
    ss << val.Dvalue;
    std::string s = ss.str();
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (!s.empty() && s.back() == '.') s.pop_back();
    }
    return s + val.Svalue;
}

Value CHTLGenerator::resolvePropertyValue(const std::vector<PropertyValue>& parts) {
    if (parts.empty()) return {0, "", ""};
    if (parts.size() == 1) {
        const auto& part = parts[0];
        if (std::holds_alternative<Token>(part)) {
            const auto& token = std::get<Token>(part);
            if (token.type == TokenType::StringLiteral || token.type == TokenType::UnquotedLiteral || token.type == TokenType::Identifier) {
                return {0, "", token.lexeme};
            }
             if (token.type == TokenType::Number) {
                return {std::stod(token.lexeme), "", ""};
            }
        } else if (std::holds_alternative<ResponsiveValueNode>(part)) {
            const auto& responsive_node = std::get<ResponsiveValueNode>(part);
            responsive_variables_.insert(responsive_node.getVariableName());
            return {0, "", "var(--" + responsive_node.getVariableName() + ")"};
        }
    }
    auto it = parts.begin();
    return evaluateExpression(it, parts.end(), 0);
}

bool elementMatchesSimpleSelector(const ElementNode* elem, const std::string& simple_selector) {
    if (!elem || simple_selector.empty()) return false;
    std::string selector = simple_selector;
    int index = -1;
    size_t bracket_pos = selector.find('[');
    if (bracket_pos != std::string::npos) {
        size_t end_bracket_pos = selector.find(']');
        if (end_bracket_pos == std::string::npos) throw std::runtime_error("Mismatched brackets in selector.");
        try {
            index = std::stoi(selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid index in selector.");
        }
        selector = selector.substr(0, bracket_pos);
    }
    bool matches = false;
    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        for (const auto& attr : elem->attributes_) {
            if (attr->key_ == "id" && attr->value_ == id) {
                matches = true;
                break;
            }
        }
    } else if (selector[0] == '.') {
        std::string className = selector.substr(1);
        for (const auto& attr : elem->attributes_) {
            if (attr->key_ == "class") {
                std::stringstream ss(attr->value_);
                std::string current_class;
                while (ss >> current_class) {
                    if (current_class == className) {
                        matches = true;
                        break;
                    }
                }
            }
            if (matches) break;
        }
    } else {
        if (elem->tagName_ == selector) matches = true;
        if (!matches) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "id" && attr->value_ == selector) {
                    matches = true;
                    break;
                }
            }
        }
        if (!matches) {
            for (const auto& attr : elem->attributes_) {
                if (attr->key_ == "class") {
                     if (attr->value_.find(selector) != std::string::npos) {
                         matches = true;
                         break;
                     }
                }
            }
        }
    }
    return matches;
}

void findAllDescendants(const ElementNode* parent, std::vector<ElementNode*>& descendants) {
    if (!parent) return;
    for (const auto& child : parent->children_) {
        if (child->getType() == NodeType::Element) {
            auto* child_elem = static_cast<ElementNode*>(child.get());
            descendants.push_back(child_elem);
            findAllDescendants(child_elem, descendants);
        }
    }
}

ElementNode* findElementBySelector(const std::string& selector, const std::vector<ElementNode*>& elements) {
    std::stringstream ss(selector);
    std::string segment;
    std::vector<std::string> segments;
    while (ss >> segment) {
        segments.push_back(segment);
    }
    if (segments.empty()) return nullptr;
    std::vector<ElementNode*> current_search_space = elements;
    std::vector<ElementNode*> next_search_space;
    ElementNode* final_match = nullptr;
    int index_to_find = -1;
    for (size_t i = 0; i < segments.size(); ++i) {
        std::string current_segment = segments[i];
        size_t bracket_pos = current_segment.find('[');
        if (bracket_pos != std::string::npos) {
            size_t end_bracket_pos = current_segment.find(']');
            if (end_bracket_pos == std::string::npos) throw std::runtime_error("Mismatched brackets in selector.");
            index_to_find = std::stoi(current_segment.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
            current_segment = current_segment.substr(0, bracket_pos);
        } else {
            index_to_find = -1;
        }
        int current_match_count = 0;
        final_match = nullptr;
        next_search_space.clear();
        for (auto* elem : current_search_space) {
            if (elementMatchesSimpleSelector(elem, current_segment)) {
                 if (index_to_find == -1 || current_match_count == index_to_find) {
                    final_match = elem;
                    if (i < segments.size() - 1) {
                        findAllDescendants(elem, next_search_space);
                    } else {
                         return final_match;
                    }
                }
                current_match_count++;
            }
        }
        if (final_match == nullptr) return nullptr;
        current_search_space = next_search_space;
    }
    return final_match;
}

CompilationResult CHTLGenerator::generate(RootNode& root, bool use_default_struct) {
    // Clear state from previous runs
    output_.str("");
    output_.clear();
    global_styles_.str("");
    global_styles_.clear();
    global_scripts_.str("");
    global_scripts_.clear();
    all_elements_.clear();
    symbol_table_.clear();
    unresolved_properties_.clear();
    responsive_variables_.clear();
    placeholder_map_.clear();
    indentLevel_ = 0;

    for (auto& child : root.children_) firstPass(child.get());
    secondPass();
    for (const auto& child : root.children_) render(child.get());

    std::string html_body = output_.str();
    std::string final_html;

    std::string styles = global_styles_.str();
    std::string scripts = global_scripts_.str();
    std::string reactivity_script = generateReactivityScript();
    if (!reactivity_script.empty()) {
        scripts = reactivity_script + "\n" + scripts;
    }

    if (use_default_struct) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html>\n";
        ss << "<head>\n";
        ss << "  <meta charset=\"UTF-8\">\n";
        ss << "  <title>CHTL Output</title>\n";
        if (!styles.empty()) {
            ss << "  <style>\n" << styles << "  </style>\n";
        }
        ss << "</head>\n";
        ss << "<body>\n";
        ss << html_body;
        if (!scripts.empty()) {
            ss << "  <script>\n" << scripts << "  </script>\n";
        }
        ss << "</body>\n";
        ss << "</html>";
        final_html = ss.str();
    } else {
        // Just return the raw parts, let the caller combine them.
        // For CLI, we'll still combine them.
        if (!styles.empty()) {
            html_body = "<style>\n" + styles + "</style>\n" + html_body;
        }
        if (!scripts.empty()) {
            html_body += "\n<script>\n" + scripts + "</script>\n";
        }
        final_html = html_body;
    }

    return {final_html, scripts, placeholder_map_};
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

std::string CHTLGenerator::generateReactivityScript() {
    if (responsive_variables_.empty()) {
        return "";
    }
    std::stringstream ss;
    ss << "(function() {\n";
    ss << "  const root = document.documentElement;\n";
    for (const auto& var_name : responsive_variables_) {
        ss << "  let _" << var_name << ";\n";
        ss << "  Object.defineProperty(window, '" << var_name << "', {\n";
        ss << "    get: () => _" << var_name << ",\n";
        ss << "    set: (newValue) => {\n";
        ss << "      _" << var_name << " = newValue;\n";
        ss << "      root.style.setProperty('--" << var_name << "', typeof newValue === 'string' ? newValue : newValue + 'px');\n";
        ss << "    }\n";
        ss << "  });\n";
    }
    ss << "})();";
    return ss.str();
}

void CHTLGenerator::render(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element: renderElement(static_cast<const ElementNode*>(node)); break;
        case NodeType::Text: renderText(static_cast<const TextNode*>(node)); break;
        case NodeType::Comment: renderComment(static_cast<const CommentNode*>(node)); break;
        case NodeType::Origin: renderOrigin(static_cast<const OriginNode*>(node)); break;
        case NodeType::StyleBlock: break; // Handled in first pass
        case NodeType::ScriptBlock: renderScriptBlock(static_cast<const ScriptBlockNode*>(node)); break;
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

void CHTLGenerator::renderScriptBlock(const ScriptBlockNode* node) {
    if (node->content_.empty()) {
        return;
    }

    // 1. Scan the raw script content to separate JS from CHTL JS
    CHTLUnifiedScanner scanner(node->content_);
    std::vector<CodeFragment> fragments = scanner.scan();

    // 2. Process the fragments
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::JS) {
            // Pure JS fragments are appended directly
            global_scripts_ << fragment.content;
        } else if (fragment.type == FragmentType::CHTL_JS) {
            // CHTL_JS fragments need to be compiled
            try {
                auto chtljs_context = std::make_shared<CHTLJS::CHTLJSContext>();
                CHTLJS::CHTLJSLexer js_lexer(fragment.content);
                std::vector<CHTLJS::CHTLJSToken> js_tokens = js_lexer.scanTokens();

                if (!js_tokens.empty() && !(js_tokens.size() == 1 && js_tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) {
                    CHTLJS::CHTLJSParser js_parser(js_tokens, chtljs_context);
                    std::unique_ptr<CHTLJS::SequenceNode> js_ast = js_parser.parse();

                    if (js_ast) {
                        CHTLJS::CHTLJSGenerator js_generator;
                        // The placeholder map is empty because the new scanner doesn't create them for script blocks
                        std::map<std::string, std::string> empty_map;
                        global_scripts_ << js_generator.generate(*js_ast, empty_map);
                    }
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("CHTLJS compilation error in script block: " + std::string(e.what()));
            }
        }
    }
    global_scripts_ << "\n";
}

}
