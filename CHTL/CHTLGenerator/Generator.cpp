#include "Generator.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/CustomStyleTemplateNode.h"
#include <iostream>

Generator::Generator(Parser& parser) : parser(parser) {}

std::string Generator::generate(ElementNode& root) {
    for (auto& child : root.children) {
        child->accept(*this);
    }
    return output.str();
}

void Generator::visit(ElementNode& node) {
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    std::stringstream style_ss;
    for (const auto& child : node.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->properties) {
                style_ss << prop.first << ":" << prop.second << ";";
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

    if (node.children.empty() && isSelfClosing) {
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
