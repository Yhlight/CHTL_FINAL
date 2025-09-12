#include "CHTLGenerator.h"
#include <map>

namespace CHTL {

// Helper to render a property value, which might be a simple literal or a complex expression.
std::string renderCssValue(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return "";
    }

    bool has_operator = false;
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Plus:
            case TokenType::Minus:
            case TokenType::Asterisk:
            case TokenType::Slash:
            case TokenType::Percent:
            case TokenType::DoubleAsterisk:
                has_operator = true;
                break;
            default:
                break;
        }
        if (has_operator) break;
    }

    std::stringstream ss;
    if (has_operator) {
        ss << "calc(";
    }

    for (size_t i = 0; i < tokens.size(); ++i) {
        ss << tokens[i].lexeme;
        // Add space between tokens, except for the last one.
        if (i < tokens.size() - 1) {
            ss << " ";
        }
    }

    if (has_operator) {
        ss << ")";
    }

    return ss.str();
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

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "  "; // 2 spaces for indentation
    }
}

} // namespace CHTL
