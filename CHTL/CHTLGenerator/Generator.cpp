#include "Generator.h"
#include "CHTL/CHTLParser/Parser.h"
#include <iostream>

Generator::Generator(Parser& parser) : parser(parser) {}

std::string Generator::generate(ElementNode& root) {
    // The root node from the parser is a dummy node.
    // We should generate its children.
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

    // Collect inline styles from StyleNode children
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

    // Check for self-closing tags, a simple list for now.
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
    // This will be implemented in the next step.
    // For now, we do nothing.
}

void Generator::visit(TemplateUsageNode& node) {
    if (node.type == TemplateType::ELEMENT) {
        if (parser.elementTemplates.count(node.name)) {
            const auto& tmpl = parser.elementTemplates.at(node.name);
            for (const auto& child : tmpl->children) {
                child->accept(*this);
            }
        } else {
            std::cerr << "Generator Error: Element template '" << node.name << "' not found." << std::endl;
        }
    }
}

void Generator::visit(OriginNode& node) {
    output << node.rawContent;
}
