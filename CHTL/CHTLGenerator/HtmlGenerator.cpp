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

// ... (rest of the file remains the same)

// Basic HTML escaping
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


// First pass to collect templates
class TemplateCollector {
public:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> styleTemplates;

    void collect(const std::vector<NodePtr>& nodes) {
        for (const auto& node : nodes) {
            collectNode(node);
        }
    }
private:
    void collectNode(const NodePtr& node) {
        if (auto def = std::dynamic_pointer_cast<TemplateDefinitionNode>(node)) {
            if (def->templateType == TemplateType::ELEMENT) {
                elementTemplates[def->name] = def;
            } else if (def->templateType == TemplateType::STYLE) {
                styleTemplates[def->name] = def;
            }
        } else if (auto elem = std::dynamic_pointer_cast<ElementNode>(node)) {
            // Recurse on children to find nested definitions
            for (const auto& child : elem->children) {
                collectNode(child);
            }
        }
    }
};


std::string HtmlGenerator::generate(const std::vector<NodePtr>& nodes) {
    // 1st Pass: Collect all template definitions
    TemplateCollector collector;
    collector.collect(nodes);
    m_elementTemplates = collector.elementTemplates;
    m_styleTemplates = collector.styleTemplates;

    // 2nd Pass: Generate HTML
    m_result = "";
    for (const auto& node : nodes) {
        node->accept(*this);
    }
    return m_result;
}

void HtmlGenerator::indent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_result += "  "; // 2 spaces for indentation
    }
}

void HtmlGenerator::visit(ElementNode& node) {
    // List of void elements that don't need a closing tag.
    static const std::set<std::string> void_elements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    // Find style nodes and generate inline style string
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

void HtmlGenerator::visit(TextNode& node) {
    std::string text_content;
    for (size_t i = 0; i < node.textTokens.size(); ++i) {
        text_content += node.textTokens[i].lexeme;
        if (i < node.textTokens.size() - 1) {
            text_content += " ";
        }
    }

    indent();
    m_result += escapeHtml(text_content) + "\n";
}

void HtmlGenerator::visit(AttributeNode& node) {
    std::string value_content;
    for (size_t i = 0; i < node.valueTokens.size(); ++i) {
        value_content += node.valueTokens[i].lexeme;
        if (i < node.valueTokens.size() - 1) {
            value_content += " ";
        }
    }
    m_result += node.name + "=\"" + escapeHtml(value_content) + "\"";
}

void HtmlGenerator::visit(CommentNode& node) {
    indent();
    std::string content = node.content;
    if (content.rfind("--", 0) == 0) {
        content = content.substr(2);
    }
    size_t first = content.find_first_not_of(" \t");
    if (std::string::npos != first) {
        content = content.substr(first);
    }

    m_result += "<!-- " + escapeHtml(content) + " -->\n";
}

void HtmlGenerator::visit(StyleNode& node) {
    // Handled by parent ElementNode
}

void HtmlGenerator::visit(TemplateDefinitionNode& node) {
    // Handled in first pass
}

void HtmlGenerator::visit(TemplateUsageNode& node) {
    if (node.type.lexeme == "Element") {
        if (m_elementTemplates.count(node.name)) {
            auto def = m_elementTemplates[node.name];
            for (const auto& bodyNode : def->bodyNodes) {
                bodyNode->accept(*this);
            }
        }
    }
    // @Style usage is handled by the parent ElementNode
}

void HtmlGenerator::visit(StyleRuleNode& node) {
    // Handled by grandparent ElementNode
}

void HtmlGenerator::visit(CustomDefinitionNode& node) {
    // Custom definitions are also collected in a first pass.
}

void HtmlGenerator::visit(ImportNode& node) {
    // Imports are handled by the parser and do not generate direct output.
}

void HtmlGenerator::visit(NamespaceNode& node) {
    // A namespace block itself doesn't create an element, but its children do.
    for (const auto& child : node.body) {
        child->accept(*this);
    }
}

} // namespace CHTL
