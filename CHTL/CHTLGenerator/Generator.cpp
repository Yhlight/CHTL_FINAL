#include "Generator.h"
#include "../CHTLEvaluator/Evaluator.h"
#include "../CHTLNode/StyleBlockNode.h"
#include <sstream>
#include <algorithm>

// Initialize the static set of self-closing tags
const std::set<std::string> Generator::s_selfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

std::string Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& program) {
    m_global_styles.str(""); // Clear global styles at the start
    std::stringstream ss;
    for (const auto& node : program) {
        ss << generateNode(node.get());
    }

    std::string final_html = ss.str();
    std::string styles = m_global_styles.str();

    if (!styles.empty()) {
        size_t head_pos = final_html.find("</head>");
        if (head_pos != std::string::npos) {
            final_html.insert(head_pos, "<style>\n" + styles + "</style>\n");
        } else {
            // Fallback: inject at the beginning if no head tag found
            final_html = "<style>\n" + styles + "</style>\n" + final_html;
        }
    }

    return final_html;
}

std::string Generator::generateNode(const BaseNode* node) {
    if (!node) {
        return "";
    }

    // Use dynamic_cast to determine the concrete type of the node
    // We cast away const here because the generator might need to modify the node (e.g., add class attribute)
    if (auto elementNode = dynamic_cast<const ElementNode*>(node)) {
        return generateElement(const_cast<ElementNode*>(elementNode));
    }
    if (auto textNode = dynamic_cast<const TextNode*>(node)) {
        return generateText(textNode);
    }
    if (auto commentNode = dynamic_cast<const CommentNode*>(node)) {
        return generateComment(commentNode);
    }
    if (auto styleBlockNode = dynamic_cast<const StyleBlockNode*>(node)) {
        // Style blocks are handled by their parent element, so they generate no output themselves.
        return "";
    }

    // If we have an unknown node type, we return an empty string.
    return "";
}

void Generator::processStyleBlock(ElementNode* node) {
    Evaluator evaluator;

    for (const auto& child : node->children) {
        if (auto styleNode = dynamic_cast<const StyleBlockNode*>(child.get())) {
            // Process global CSS rules
            for (const auto& rule : styleNode->rules) {
                std::string selector = rule->selector;
                // Auto-add class/id attributes
                if (selector.rfind(".", 0) == 0) { // Starts with .
                    std::string className = selector.substr(1);
                    bool found = false;
                    for (const auto& attr : node->attributes) {
                        if (attr->name == "class") {
                            // A simple check; doesn't handle multiple classes in the attribute well
                            if (attr->value.find(className) == std::string::npos) {
                                attr->value += " " + className;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        node->addAttribute(std::make_unique<AttributeNode>(Token(), "class", className));
                    }
                }
                // Simplified '&' replacement
                if (selector.rfind("&", 0) == 0) {
                    // Find parent's primary selector (e.g., first class)
                     for (const auto& attr : node->attributes) {
                        if (attr->name == "class") {
                            std::string firstClass = attr->value.substr(0, attr->value.find(" "));
                            selector.replace(0, 1, "." + firstClass);
                            break;
                        }
                    }
                }

                m_global_styles << selector << " {\n";
                for (const auto& prop : rule->properties) {
                    m_global_styles << "  " << prop->propertyName << ": " << evaluator.evaluate(prop->value.get()) << ";\n";
                }
                m_global_styles << "}\n";
            }
        }
    }
}


std::string Generator::generateElement(const ElementNode* const_node) {
    ElementNode* node = const_cast<ElementNode*>(const_node);

    // Process style blocks first to potentially add attributes
    processStyleBlock(node);

    std::stringstream ss;
    ss << "<" << node->tagName;

    // Append attributes
    for (const auto& attr : node->attributes) {
        ss << " " << attr->name << "=\"" << attr->value << "\"";
    }

    // Find and process inline styles
    std::stringstream style_ss;
    Evaluator evaluator;
    for (const auto& child : node->children) {
        if (auto styleNode = dynamic_cast<const StyleBlockNode*>(child.get())) {
            for (const auto& prop : styleNode->inline_properties) {
                style_ss << prop->propertyName << ":" << evaluator.evaluate(prop->value.get()) << ";";
            }
        }
    }
    std::string style_attr = style_ss.str();
    if (!style_attr.empty()) {
        ss << " style=\"" << style_attr << "\"";
    }


    // Handle self-closing tags
    if (s_selfClosingTags.count(node->tagName)) {
        ss << " />";
    } else {
        ss << ">";
        // Append children (excluding style blocks which have been processed)
        for (const auto& child : node->children) {
            if (!dynamic_cast<const StyleBlockNode*>(child.get())) {
                 ss << generateNode(child.get());
            }
        }
        ss << "</" << node->tagName << ">";
    }

    return ss.str();
}

// Helper function to escape special HTML characters
std::string escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&#39;");       break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

std::string Generator::generateText(const TextNode* node) {
    // HTML-escape the content to prevent XSS
    return escapeHTML(node->content);
}

std::string Generator::generateComment(const CommentNode* node) {
    // Only generator comments (--) are rendered into HTML
    if (node->token.type == TokenType::GENERATOR_COMMENT) {
        // Strip the leading "--" from the lexeme
        std::string content = node->content.substr(2);
        // Trim leading space if it exists
        if (!content.empty() && content[0] == ' ') {
            content = content.substr(1);
        }
        return "<!-- " + content + " -->";
    }
    // Other comment types are ignored
    return "";
}
