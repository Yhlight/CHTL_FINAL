#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLEvaluator/Evaluator.h"
#include <sstream>
#include <algorithm>

std::string Generator::generate(std::shared_ptr<RootNode> ast) {
    html.clear();
    globalCss.str("");
    globalCss.clear();
    ast->accept(*this);

    // Inject global CSS into the head
    std::string finalHtml = html;
    if (globalCss.tellp() > 0) {
        std::string styleBlock = "\n<style>\n" + globalCss.str() + "</style>\n";
        size_t head_pos = finalHtml.find("</head>");
        if (head_pos != std::string::npos) {
            finalHtml.insert(head_pos, styleBlock);
        } else {
            // If no head, just prepend (not ideal, but a fallback)
            finalHtml.insert(0, styleBlock);
        }
    }
    return finalHtml;
}

void Generator::indent() {
    html += std::string(indentLevel * 2, ' ');
}

void Generator::visit(RootNode& node) {
    html += "<!DOCTYPE html>\n";
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    html += "<" + node.tagName;

    std::stringstream styleStream;
    Evaluator evaluator;
    bool hasClassAttr = false;
    bool hasIdAttr = false;

    for(const auto& attr : node.attributes) {
        if (attr->key == "class") hasClassAttr = true;
        if (attr->key == "id") hasIdAttr = true;
    }

    for (const auto& child : node.children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            for (const auto& prop : styleNode->properties) {
                Value result = evaluator.evaluate(prop->value);
                styleStream << prop->key << ": " << result.toString() << "; ";
            }
            for (auto& rule : styleNode->rules) {
                std::string originalSelector = rule->selector; // Keep original for reuse
                if (rule->selector[0] == '.' && !hasClassAttr) {
                    node.addAttribute(std::make_shared<AttributeNode>("class", rule->selector.substr(1)));
                    hasClassAttr = true;
                } else if (rule->selector[0] == '#' && !hasIdAttr) {
                    node.addAttribute(std::make_shared<AttributeNode>("id", rule->selector.substr(1)));
                    hasIdAttr = true;
                }

                if (rule->selector[0] == '&') {
                    std::string parentSelector;
                    auto idAttr = std::find_if(node.attributes.begin(), node.attributes.end(), [](const auto& attr){ return attr->key == "id"; });
                    auto classAttr = std::find_if(node.attributes.begin(), node.attributes.end(), [](const auto& attr){ return attr->key == "class"; });

                    if (idAttr != node.attributes.end()) {
                        parentSelector = "#" + (*idAttr)->value;
                    } else if (classAttr != node.attributes.end()) {
                        parentSelector = "." + (*classAttr)->value;
                    } else {
                        parentSelector = node.tagName;
                    }
                    rule->selector.replace(0, 1, parentSelector);
                }

                rule->accept(*this);
                rule->selector = originalSelector; // Restore selector
            }
        }
    }

    for (auto& attr : node.attributes) {
        attr->accept(*this);
    }

    std::string styleString = styleStream.str();
    if (!styleString.empty()) {
        styleString.pop_back();
        html += " style=\"" + styleString + "\"";
    }

    html += ">\n";
    indentLevel++;

    for (auto& child : node.children) {
        if (std::dynamic_pointer_cast<StyleNode>(child)) continue;
        child->accept(*this);
    }

    indentLevel--;
    indent();
    html += "</" + node.tagName + ">\n";
}

void Generator::visit(AttributeNode& node) {
    html += " " + node.key + "=\"" + node.value + "\"";
}

void Generator::visit(TextNode& node) {
    indent();
    html += node.text + "\n";
}

void Generator::visit(StyleNode& node) {}

void Generator::visit(PropertyNode& node) {}

void Generator::visit(RuleNode& node) {
    Evaluator evaluator;
    globalCss << node.selector << " {\n";
    for (const auto& prop : node.properties) {
        Value result = evaluator.evaluate(prop->value);
        globalCss << "  " << prop->key << ": " << result.toString() << ";\n";
    }
    globalCss << "}\n";
}
