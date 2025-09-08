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
