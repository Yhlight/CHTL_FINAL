#include "HtmlGenerator.hpp"
#include "../CHTLNode/ElementNode.hpp"
#include "../CHTLNode/TextNode.hpp"
#include "../CHTLNode/AttributeNode.hpp"
#include "../CHTLNode/CommentNode.hpp"
#include "../CHTLNode/StyleNode.hpp"
#include "../CHTLNode/TemplateUsageNode.hpp"
#include "../CHTLNode/StyleRuleNode.hpp"
#include "../CHTLNode/CustomDefinitionNode.hpp"
#include "../CHTLNode/ImportNode.hpp"
#include "../CHTLNode/NamespaceNode.hpp"
#include "../CSSExt/ExpressionParser.hpp"
#include "../CSSExt/ExpressionEvaluator.hpp"
#include <set>
#include <sstream>

namespace CHTL {

namespace { // Anonymous namespace for helper functions

void processStyleRule(const StyleRule& rule_tokens, std::string& inline_style) {
    if (rule_tokens.size() >= 2 && rule_tokens[1].type == TokenType::COLON) {
        std::string key = rule_tokens[0].lexeme;
        std::vector<Token> value_tokens;
        for(size_t i = 2; i < rule_tokens.size(); ++i) {
            if (rule_tokens[i].type == TokenType::SEMICOLON) break;
            value_tokens.push_back(rule_tokens[i]);
        }
        if (value_tokens.empty()) return;
        CSSExt::ExpressionParser parser(value_tokens);
        CSSExt::ExprPtr expr = parser.parse();
        if (!expr) return;
        CSSExt::ExpressionEvaluator evaluator;
        CSSExt::Value val = evaluator.evaluate(expr);
        std::stringstream ss;
        if (val.is_number) {
            ss << val.number << val.unit;
        } else {
            ss << val.string_val;
        }
        inline_style += key + ": " + ss.str() + "; ";
    }
}

} // namespace

std::string escapeHtml(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

std::string HtmlGenerator::generate(const std::vector<NodePtr>& nodes, CHTLContext& context) {
    m_elementTemplates = context.elementTemplates;
    m_styleTemplates = context.styleTemplates;
    m_result = "";
    for (const auto& node : nodes) {
        node->accept(*this);
    }
    return m_result;
}

void HtmlGenerator::indent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_result += "  ";
    }
}

void HtmlGenerator::visit(ElementNode& node) {
    static const std::set<std::string> void_elements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    std::string inline_style;
    for (const auto& child : node.children) {
        if (auto style_node = std::dynamic_pointer_cast<StyleNode>(child)) {
            for (const auto& style_content_node : style_node->children) {
                if (auto rule_node = std::dynamic_pointer_cast<StyleRuleNode>(style_content_node)) {
                    processStyleRule(rule_node->rule, inline_style);
                } else if (auto usage_node = std::dynamic_pointer_cast<TemplateUsageNode>(style_content_node)) {
                    if (m_styleTemplates.count(usage_node->name)) {
                        auto def = m_styleTemplates[usage_node->name];
                        for (const auto& rule_tokens : def->styleRules) {
                            processStyleRule(rule_tokens, inline_style);
                        }
                    }
                }
            }
        }
    }

    indent();
    m_result += "<" + node.tagName;
    for (const auto& attr : node.attributes) {
        m_result += " ";
        attr->accept(*this);
    }
    if (!inline_style.empty()) {
        m_result += " style=\"" + inline_style + "\"";
    }
    m_result += ">\n";

    bool is_void = void_elements.count(node.tagName) > 0;
    if (!is_void) {
        m_indent_level++;
        for (const auto& child : node.children) {
            if (!std::dynamic_pointer_cast<StyleNode>(child)) {
                child->accept(*this);
            }
        }
        m_indent_level--;
        indent();
        m_result += "</" + node.tagName + ">\n";
    }
}

// ... other visit methods ...
void HtmlGenerator::visit(TextNode& node) {
    std::string text_content;
    for (size_t i = 0; i < node.textTokens.size(); ++i) {
        text_content += node.textTokens[i].lexeme;
        if (i < node.textTokens.size() - 1) text_content += " ";
    }
    indent();
    m_result += escapeHtml(text_content) + "\n";
}

void HtmlGenerator::visit(AttributeNode& node) {
    std::string value_content;
    for (size_t i = 0; i < node.valueTokens.size(); ++i) {
        value_content += node.valueTokens[i].lexeme;
        if (i < node.valueTokens.size() - 1) value_content += " ";
    }
    m_result += node.name + "=\"" + escapeHtml(value_content) + "\"";
}

void HtmlGenerator::visit(CommentNode& node) {
    indent();
    std::string content = node.content;
    if (content.rfind("--", 0) == 0) content = content.substr(2);
    size_t first = content.find_first_not_of(" \t");
    if (std::string::npos != first) content = content.substr(first);
    m_result += "<!-- " + escapeHtml(content) + " -->\n";
}

void HtmlGenerator::visit(StyleNode& node) {}
void HtmlGenerator::visit(TemplateDefinitionNode& node) {}
void HtmlGenerator::visit(CustomDefinitionNode& node) {}
void HtmlGenerator::visit(StyleRuleNode& node) {}

void HtmlGenerator::visit(TemplateUsageNode& node) {
    if (node.type.lexeme == "Element") {
        if (m_elementTemplates.count(node.name)) {
            auto def = m_elementTemplates[node.name];
            for (const auto& bodyNode : def->bodyNodes) {
                bodyNode->accept(*this);
            }
        }
    }
}

void HtmlGenerator::visit(ImportNode& node) {}
void HtmlGenerator::visit(NamespaceNode& node) {
    for (const auto& child : node.body) {
        child->accept(*this);
    }
}

}
