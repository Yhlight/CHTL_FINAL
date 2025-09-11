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
#include "../CHTLNode/DeleteNode.hpp"
#include "../CHTLNode/InsertNode.hpp"
#include "../CSSExt/ExpressionParser.hpp"
#include "../CSSExt/ExpressionEvaluator.hpp"
#include <set>
#include <sstream>
#include <algorithm>

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
    if (!node.valueExpression) {
        m_result += node.name + "=\"\"";
        return;
    }

    CSSExt::ExpressionEvaluator evaluator;
    CSSExt::Value val = evaluator.evaluate(node.valueExpression);

    std::stringstream ss;
    if (val.is_number) {
        ss << val.number << val.unit;
    } else {
        ss << val.string_val;
    }

    m_result += node.name + "=\"" + escapeHtml(ss.str()) + "\"";
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

            // Start with a copy of the template's body
            auto finalBodyNodes = def->bodyNodes;

            // Process specializations
            for (const auto& specNode : node.specializationBody) {
                if (auto deleteNode = std::dynamic_pointer_cast<DeleteNode>(specNode)) {
                    // Handle delete instructions
                    std::vector<std::string> targets_to_delete;
                    for(const auto& token : deleteNode->targets) {
                        targets_to_delete.push_back(token.lexeme);
                    }

                    finalBodyNodes.erase(
                        std::remove_if(finalBodyNodes.begin(), finalBodyNodes.end(),
                            [&](const NodePtr& bodyNode){
                                if (auto elem = std::dynamic_pointer_cast<ElementNode>(bodyNode)) {
                                    for (const auto& target : targets_to_delete) {
                                        if (elem->tagName == target) return true;
                                    }
                                }
                                return false;
                            }),
                        finalBodyNodes.end());
                }
                else if (auto insertNode = std::dynamic_pointer_cast<InsertNode>(specNode)) {
                    // Find the target node to insert before/after
                    auto it = std::find_if(finalBodyNodes.begin(), finalBodyNodes.end(),
                        [&](const NodePtr& bodyNode){
                            if (auto elem = std::dynamic_pointer_cast<ElementNode>(bodyNode)) {
                                // Simplified selector logic
                                return elem->tagName == insertNode->selector;
                            }
                            return false;
                        });

                    if (it != finalBodyNodes.end()) {
                        if (insertNode->position.type == TokenType::BEFORE) {
                            finalBodyNodes.insert(it, insertNode->nodesToInsert.begin(), insertNode->nodesToInsert.end());
                        } else if (insertNode->position.type == TokenType::AFTER) {
                            finalBodyNodes.insert(it + 1, insertNode->nodesToInsert.begin(), insertNode->nodesToInsert.end());
                        }
                    }
                }
            }

            // Visit the final, specialized body
            for (const auto& bodyNode : finalBodyNodes) {
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

void HtmlGenerator::visit(DeleteNode& node) {
    // This logic will be handled by the parent node that is being specialized.
}

void HtmlGenerator::visit(InsertNode& node) {
    // This logic will be handled by the parent node that is being specialized.
}

}
