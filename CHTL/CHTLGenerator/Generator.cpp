#include "Generator.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/StyleRuleNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include "CHTL/CHTLJS/Preprocessor.h"
#include <iostream>
#include <algorithm>

Generator::Generator(Parser& parser, std::shared_ptr<ConfigurationState> config) : parser(parser) {}

std::string Generator::generate(ElementNode& root) {
    for (auto& child : root.children) {
        child->accept(*this);
    }

    std::string main_output = output.str();
    std::string css_output = global_css.str();
    std::string js_output = global_js.str();

    if (!css_output.empty()) {
        std::string style_block = "<style>\n" + css_output + "</style>\n";
        size_t head_pos = main_output.find("</head>");
        if (head_pos != std::string::npos) {
            main_output.insert(head_pos, style_block);
        } else {
            main_output = style_block + main_output;
        }
    }

    if (!js_output.empty()) {
        std::string script_block = "<script>\n"
                                 "document.addEventListener('DOMContentLoaded', function() {\n"
                                 + js_output
                                 + "});\n</script>\n";
        size_t body_pos = main_output.find("</body>");
        if (body_pos != std::string::npos) {
            main_output.insert(body_pos, script_block);
        } else {
            main_output += script_block;
        }
    }

    return main_output;
}

void Generator::visit(ElementNode& node) {
    auto* oldContext = currentElement;
    currentElement = &node;

    // --- Pre-computation for styles ---
    for (const auto& child : node.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->rules) {
                std::string final_selector = rule->selector;
                if (rule->selector[0] == '.') {
                    if (node.attributes.find("class") == node.attributes.end()) node.attributes["class"] = rule->selector.substr(1);
                    else if (node.attributes["class"].find(rule->selector.substr(1)) == std::string::npos) node.attributes["class"] += " " + rule->selector.substr(1);
                } else if (rule->selector[0] == '#') {
                    if (node.attributes.find("id") == node.attributes.end()) node.attributes["id"] = rule->selector.substr(1);
                } else if (rule->selector[0] == '&') {
                    std::string parent_selector;
                    if (node.attributes.count("id")) parent_selector = "#" + node.attributes["id"];
                    else if (node.attributes.count("class")) parent_selector = "." + node.attributes["class"].substr(0, node.attributes["class"].find(" "));
                    if (!parent_selector.empty()) final_selector.replace(0, 1, parent_selector);
                }
                global_css << final_selector << " {\n";
                for (const auto& prop : rule->properties) global_css << "    " << prop.first << ": " << prop.second << ";\n";
                global_css << "}\n";
            }
        }
    }

    // --- Generation Step ---
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Handle inline styles by evaluating expressions
    std::stringstream style_ss;
    for (const auto& child : node.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->inlineProperties) {
                Value result = evaluateExpression(*prop.second);
                style_ss << prop.first << ":" << result.string << ";";
            }
        }
    }
    if (style_ss.rdbuf()->in_avail() > 0) {
        output << " style=\"" << style_ss.str() << "\"";
    }

    std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), node.tagName) != selfClosingTags.end();

    bool hasVisibleChildren = false;
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr && dynamic_cast<ScriptNode*>(child.get()) == nullptr) {
            hasVisibleChildren = true;
            break;
        }
    }

    if (!hasVisibleChildren && isSelfClosing) {
        output << " />";
        currentElement = oldContext;
        return;
    }

    output << ">";

    for (auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr && dynamic_cast<ScriptNode*>(child.get()) == nullptr) {
            child->accept(*this);
        } else {
            // Also visit script nodes to populate the global_js buffer
            if (dynamic_cast<ScriptNode*>(child.get()) != nullptr) {
                 child->accept(*this);
            }
        }
    }

    output << "</" << node.tagName << ">";
    currentElement = oldContext;
}

void Generator::visit(TextNode& node) { output << node.content; }
void Generator::visit(StyleNode& node) { /* Handled in visit(ElementNode&) */ }
void Generator::visit(CustomStyleTemplateNode& node) { /* Declaration node, nothing to generate. */ }
void Generator::visit(StyleRuleNode& node) { /* Handled in visit(ElementNode&) */ }

void Generator::visit(TemplateUsageNode& node) {
    if (node.type == TemplateType::ELEMENT) {
        const ElementTemplateNode* tmpl = nullptr;
        if (parser.nsElementTemplates[node.ns].count(node.name)) tmpl = parser.nsElementTemplates[node.ns].at(node.name).get();
        else if (parser.nsElementTemplates["::global"].count(node.name)) tmpl = parser.nsElementTemplates["::global"].at(node.name).get();

        const CustomElementNode* customTmpl = nullptr;
        if (parser.nsCustomElementTemplates[node.ns].count(node.name)) customTmpl = parser.nsCustomElementTemplates[node.ns].at(node.name).get();
        else if (parser.nsCustomElementTemplates["::global"].count(node.name)) customTmpl = parser.nsCustomElementTemplates["::global"].at(node.name).get();

        if (tmpl) for (const auto& child : tmpl->children) child->accept(*this);
        else if (customTmpl) for (const auto& child : customTmpl->children) child->accept(*this);
        else std::cerr << "Generator Error: Element template '" << node.name << "' not found in namespace '" << node.ns << "' or global." << std::endl;
    }
}

void Generator::visit(OriginNode& node) {
    if (!node.name_to_lookup.empty()) {
        if (parser.namedOriginBlocks.count(node.name_to_lookup)) {
            output << parser.namedOriginBlocks.at(node.name_to_lookup)->content;
        } else {
            std::cerr << "Generator Error: Named origin block '" << node.name_to_lookup << "' not found." << std::endl;
        }
    } else {
        output << node.content;
    }
}

void Generator::visit(ScriptNode& node) {
    std::string processed_script = js_preprocessor.preprocess(node.content);
    global_js << processed_script << "\n";
}

// =================================================================
// Expression Evaluation
// =================================================================

Generator::Value Generator::evaluateExpression(ExpressionNode& node) {
    if (auto* lit = dynamic_cast<LiteralNode*>(&node)) return evaluateLiteral(*lit);
    if (auto* binOp = dynamic_cast<BinaryOpNode*>(&node)) return evaluateBinaryOp(*binOp);
    if (auto* ternOp = dynamic_cast<TernaryOpNode*>(&node)) return evaluateTernaryOp(*ternOp);
    if (auto* propAccess = dynamic_cast<PropertyAccessNode*>(&node)) return evaluatePropertyAccess(*propAccess);
    std::cerr << "Generator Error: Unknown expression node type." << std::endl;
    return {0, "", false};
}

Generator::Value Generator::evaluateLiteral(LiteralNode& node) {
    try {
        size_t first_non_digit = node.value.lexeme.find_first_not_of("-.0123456789");
        if (first_non_digit == std::string::npos) return {std::stod(node.value.lexeme), node.value.lexeme, true};
    } catch (const std::invalid_argument&) { /* Not a number */ }
    return {0, node.value.lexeme, false};
}

Generator::Value Generator::evaluatePropertyAccess(PropertyAccessNode& node) {
    if (!currentElement) return {0, "", false};
    for (const auto& child : currentElement->children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            if (styleNode->inlineProperties.count(node.property.lexeme)) {
                return evaluateExpression(*styleNode->inlineProperties.at(node.property.lexeme));
            }
        }
    }
    return {0, "", false};
}

Generator::Value Generator::evaluateTernaryOp(TernaryOpNode& node) {
    Value condition = evaluateExpression(*node.condition);
    if ((condition.isNumeric && condition.number != 0) || (!condition.isNumeric && !condition.string.empty())) {
        return evaluateExpression(*node.trueExpression);
    }
    return evaluateExpression(*node.falseExpression);
}

Generator::Value Generator::evaluateBinaryOp(BinaryOpNode& node) {
    Value left = evaluateExpression(*node.left);
    Value right = evaluateExpression(*node.right);

    if (!left.isNumeric || !right.isNumeric) {
        if (node.op.type == TokenType::EQUAL_EQUAL) return {(left.string == right.string) ? 1.0 : 0.0, "", true};
        if (node.op.type == TokenType::NOT_EQUAL) return {(left.string != right.string) ? 1.0 : 0.0, "", true};
        return {0, "", false};
    }

    switch (node.op.type) {
        case TokenType::PLUS:          return {left.number + right.number, std::to_string(left.number + right.number), true};
        case TokenType::MINUS:         return {left.number - right.number, std::to_string(left.number - right.number), true};
        case TokenType::STAR:          return {left.number * right.number, std::to_string(left.number * right.number), true};
        case TokenType::SLASH:         return {left.number / right.number, std::to_string(left.number / right.number), true};
        case TokenType::GREATER:       return {(left.number > right.number) ? 1.0 : 0.0, "", true};
        case TokenType::GREATER_EQUAL: return {(left.number >= right.number) ? 1.0 : 0.0, "", true};
        case TokenType::LESS:          return {(left.number < right.number) ? 1.0 : 0.0, "", true};
        case TokenType::LESS_EQUAL:    return {(left.number <= right.number) ? 1.0 : 0.0, "", true};
        case TokenType::EQUAL_EQUAL:   return {(left.number == right.number) ? 1.0 : 0.0, "", true};
        case TokenType::NOT_EQUAL:     return {(left.number != right.number) ? 1.0 : 0.0, "", true};
        case TokenType::AND_AND:       return {(left.number != 0 && right.number != 0) ? 1.0 : 0.0, "", true};
        case TokenType::OR_OR:         return {(left.number != 0 || right.number != 0) ? 1.0 : 0.0, "", true};
        default: return {0, "", false};
    }
}
