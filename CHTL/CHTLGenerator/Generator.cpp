#include "Generator.h"
#include "../CHTLEvaluator/Evaluator.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <sstream>
#include <algorithm>

// Initialize the static set of self-closing tags
const std::set<std::string> Generator::s_selfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator(const TemplateStore& templateStore) : m_templateStore(templateStore) {}

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
    if (auto templateUsageNode = dynamic_cast<const TemplateUsageNode*>(node)) {
        return generateTemplateUsage(templateUsageNode);
    }

    // If we have an unknown node type, we return an empty string.
    return "";
}

// Helper function to process the body of a style block or style template
void Generator::processStyleBody(const std::vector<std::unique_ptr<BaseNode>>& body, ElementNode* parent, std::stringstream& inline_style_stream) {
    Evaluator evaluator(m_templateStore);
    for (const auto& node : body) {
        if (auto prop = dynamic_cast<const CSSPropertyNode*>(node.get())) {
            inline_style_stream << prop->propertyName << ":" << evaluator.evaluate(prop->value.get()) << ";";
        } else if (auto rule = dynamic_cast<const CSSRuleNode*>(node.get())) {
            // Simplified logic from before
            m_global_styles << rule->selector << " {\n";
            for (const auto& p : rule->properties) {
                m_global_styles << "  " << p->propertyName << ": " << evaluator.evaluate(p->value.get()) << ";\n";
            }
            m_global_styles << "}\n";
        } else if (auto usage = dynamic_cast<const TemplateUsageNode*>(node.get())) {
            if (usage->type == TemplateType::STYLE) {
                auto templateDef = m_templateStore.get(usage->name);
                if (templateDef) {
                    // Clone the body before recursive processing to ensure isolation.
                    std::vector<std::unique_ptr<BaseNode>> clonedBody;
                    for (const auto& n : templateDef->body) {
                        clonedBody.push_back(n->clone());
                    }
                    processStyleBody(clonedBody, parent, inline_style_stream);
                }
            }
        }
    }
}

std::string Generator::generateElement(const ElementNode* const_node) {
    ElementNode* node = const_cast<ElementNode*>(const_node);

    std::stringstream ss;
    ss << "<" << node->tagName;

    // Process style blocks to gather inline styles and global rules
    std::stringstream style_ss;
    for (const auto& child : node->children) {
        if (auto styleNode = dynamic_cast<const StyleBlockNode*>(child.get())) {
            processStyleBody(styleNode->body, node, style_ss);
        }
    }

    // Append attributes that might have been added by style processing
    for (const auto& attr : node->attributes) {
        ss << " " << attr->name << "=\"" << attr->value << "\"";
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

std::string Generator::generateTemplateUsage(const TemplateUsageNode* node) {
    if (node->type != TemplateType::ELEMENT) {
        // Only @Element templates produce direct output in the main document flow.
        // @Style templates are handled within processStyleBody.
        // @Var templates will be handled by the evaluator.
        return "";
    }

    auto templateDef = m_templateStore.get(node->name);
    if (!templateDef) {
        return "<!-- Template not found: " + node->name + " -->";
    }

    std::stringstream ss;
    for (const auto& bodyNode : templateDef->body) {
        // Clone the node before generating to ensure isolation
        ss << generateNode(bodyNode->clone().get());
    }
    return ss.str();
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
