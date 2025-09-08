#include "Generator.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/StyleRuleNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include <iostream>
#include <algorithm>

Generator::Generator(Parser& parser) : parser(parser) {}

std::string Generator::generate(ElementNode& root) {
    // Single pass to generate all content into the main output buffer.
    // The visit methods will have the side effect of populating global_css.
    for (auto& child : root.children) {
        child->accept(*this);
    }

    std::string main_output = output.str();
    std::string css_output = global_css.str();

    if (!css_output.empty()) {
        std::string style_block = "<style>\n" + css_output + "</style>\n";

        // Inject the style block after the <body> tag, or at the beginning if not found.
        size_t body_pos = main_output.find("<body>");
        if (body_pos != std::string::npos) {
            main_output.insert(body_pos + 6, style_block);
        } else {
            main_output = style_block + main_output;
        }
    }

    return main_output;
}

void Generator::visit(ElementNode& node) {
    auto* oldContext = currentElement;
    currentElement = &node;

    // --- Pre-computation Step ---
    // Process style rules before generating the tag
    for (const auto& child : node.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->rules) {
                std::string final_selector = rule->selector;

                if (rule->selector[0] == '.') {
                    std::string className = rule->selector.substr(1);
                    if (node.attributes.find("class") == node.attributes.end()) {
                        node.attributes["class"] = className;
                    } else {
                        if (node.attributes["class"].find(className) == std::string::npos) {
                            node.attributes["class"] += " " + className;
                        }
                    }
                } else if (rule->selector[0] == '#') {
                    if (node.attributes.find("id") == node.attributes.end()) {
                        node.attributes["id"] = rule->selector.substr(1);
                    }
                } else if (rule->selector[0] == '&') {
                    std::string parent_selector;
                    if (node.attributes.count("id")) {
                        parent_selector = "#" + node.attributes["id"];
                    } else if (node.attributes.count("class")) {
                        std::string first_class = node.attributes["class"].substr(0, node.attributes["class"].find(" "));
                        parent_selector = "." + first_class;
                    }

                    if (!parent_selector.empty()) {
                        final_selector.replace(0, 1, parent_selector);
                    } else {
                        std::cerr << "Warning: '&' used without a class or id on the parent element." << std::endl;
                    }
                }

                // Add rule to global css
                global_css << final_selector << " {\n";
                for (const auto& prop : rule->properties) {
                    global_css << "    " << prop.first << ": " << prop.second << ";\n";
                }
                global_css << "}\n";
            }
        }
    }

    // --- Generation Step ---
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Handle inline styles
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
    bool isSelfClosing = false;
    for(const auto& tag : selfClosingTags) {
        if (node.tagName == tag) {
            isSelfClosing = true;
            break;
        }
    }

    // Check if the node should be self-closing
    bool hasNonStyleChildren = false;
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            hasNonStyleChildren = true;
            break;
        }
    }

    if (!hasNonStyleChildren && isSelfClosing) {
        output << " />";
        return;
    }

    output << ">";

    for (auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            child->accept(*this);
        }
    }

    output << "</" << node.tagName << ">";
    currentElement = oldContext;
}

void Generator::visit(TextNode& node) {
    output << node.content;
}

void Generator::visit(StyleNode& node) {
    // Handled in visit(ElementNode&)
}

void Generator::visit(TemplateUsageNode& node) {
    if (node.type == TemplateType::ELEMENT) {
        if (parser.elementTemplates.count(node.name)) {
            const auto& tmpl = parser.elementTemplates.at(node.name);
            for (const auto& child : tmpl->children) {
                child->accept(*this);
            }
        } else if (parser.customElementTemplates.count(node.name)) {
            const auto& tmpl = parser.customElementTemplates.at(node.name);
            for (const auto& child : tmpl->children) {
                child->accept(*this);
            }
        } else {
            std::cerr << "Generator Error: Element template '" << node.name << "' not found." << std::endl;
        }
    }
}

void Generator::visit(CustomStyleTemplateNode& node) {
    // Declaration node, nothing to generate.
}

void Generator::visit(StyleRuleNode& node) {
    // Handled in visit(ElementNode&)
}

void Generator::visit(OriginNode& node) {
    output << node.content;
}

// =================================================================
// Expression Evaluation
// =================================================================

Generator::Value Generator::evaluateExpression(ExpressionNode& node) {
    if (auto* lit = dynamic_cast<LiteralNode*>(&node)) {
        return evaluateLiteral(*lit);
    }
    if (auto* binOp = dynamic_cast<BinaryOpNode*>(&node)) {
        return evaluateBinaryOp(*binOp);
    }
    if (auto* ternOp = dynamic_cast<TernaryOpNode*>(&node)) {
        return evaluateTernaryOp(*ternOp);
    }
    if (auto* propAccess = dynamic_cast<PropertyAccessNode*>(&node)) {
        return evaluatePropertyAccess(*propAccess);
    }
    std::cerr << "Generator Error: Unknown expression node type." << std::endl;
    return {0, "", false};
}

Generator::Value Generator::evaluateLiteral(LiteralNode& node) {
    try {
        size_t first_non_digit = node.value.lexeme.find_first_not_of("-.0123456789");
        if (first_non_digit == std::string::npos) {
            return {std::stod(node.value.lexeme), node.value.lexeme, true};
        }
    } catch (const std::invalid_argument&) {
        // Not a number, treat as string
    }
    return {0, node.value.lexeme, false};
}

Generator::Value Generator::evaluatePropertyAccess(PropertyAccessNode& node) {
    if (!currentElement) {
        std::cerr << "Generator Error: Cannot access property '" << node.property.lexeme << "' outside of an element context." << std::endl;
        return {0, "", false};
    }

    // For now, only look at the current element's inline styles
    for (const auto& child : currentElement->children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            if (styleNode->inlineProperties.count(node.property.lexeme)) {
                // If the property itself is an expression, evaluate it.
                return evaluateExpression(*styleNode->inlineProperties.at(node.property.lexeme));
            }
        }
    }

    std::cerr << "Generator Warning: Property '" << node.property.lexeme << "' not found on current element." << std::endl;
    return {0, "", false};
}

Generator::Value Generator::evaluateTernaryOp(TernaryOpNode& node) {
    Value condition = evaluateExpression(*node.condition);
    if (condition.isNumeric && condition.number != 0) {
        return evaluateExpression(*node.trueExpression);
    }
    if (!condition.isNumeric && !condition.string.empty()) {
        return evaluateExpression(*node.trueExpression);
    }
    return evaluateExpression(*node.falseExpression);
}

Generator::Value Generator::evaluateBinaryOp(BinaryOpNode& node) {
    Value left = evaluateExpression(*node.left);
    Value right = evaluateExpression(*node.right);

    if (!left.isNumeric || !right.isNumeric) {
        // For now, only support operations on numeric values
        std::cerr << "Generator Warning: Performing operations on non-numeric values is not fully supported." << std::endl;
        if (node.op.type == TokenType::EQUAL_EQUAL) return { (left.string == right.string) ? 1.0 : 0.0, "", true };
        if (node.op.type == TokenType::NOT_EQUAL) return { (left.string != right.string) ? 1.0 : 0.0, "", true };
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
        default:
            std::cerr << "Generator Error: Unknown binary operator '" << node.op.lexeme << "'." << std::endl;
            return {0, "", false};
    }
}
