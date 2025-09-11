#include "HtmlGenerator.hpp"
#include "../CHTLNode/ElementNode.hpp"
#include "../CHTLNode/TextNode.hpp"
#include "../CHTLNode/AttributeNode.hpp"
#include "../CHTLNode/CommentNode.hpp"
#include "../CHTLNode/StyleNode.hpp" // For dynamic_cast and member access
#include "../CSSExt/ExpressionParser.hpp"
#include "../CSSExt/ExpressionEvaluator.hpp"
#include <set>
#include <sstream>
#include <iostream>

namespace CHTL {

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


std::string HtmlGenerator::generate(const std::vector<NodePtr>& nodes) {
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
            for (const auto& rule_tokens : style_node->rules) {
                // Assuming simple key: value; rules
                if (rule_tokens.size() >= 3 && rule_tokens[1].type == TokenType::COLON) {
                    std::string key = rule_tokens[0].lexeme;

                    // Extract value tokens (from after colon to before semicolon)
                    std::vector<Token> value_tokens;
                    for(size_t i = 2; i < rule_tokens.size(); ++i) {
                        if (rule_tokens[i].type == TokenType::SEMICOLON) break;
                        value_tokens.push_back(rule_tokens[i]);
                    }

                    // DEBUG: Print tokens
                    std::cout << "DEBUG: Parsing rule '" << key << "' with tokens: ";
                    for(const auto& t : value_tokens) { std::cout << t.lexeme << " "; }
                    std::cout << std::endl;

                    // Parse and evaluate the expression
                    CSSExt::ExpressionParser parser(value_tokens);
                    CSSExt::ExprPtr expr = parser.parse();
                    CSSExt::ExpressionEvaluator evaluator;
                    CSSExt::Value val = evaluator.evaluate(expr);

                    // Format the final value
                    std::stringstream ss;
                    if (val.is_number) {
                        ss << val.number << val.unit;
                    } else {
                        ss << val.string_val;
                    }

                    inline_style += key + ": " + ss.str() + "; ";
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
        // This is a simplified approach. A better way would be to merge
        // with an existing style attribute if one exists.
        m_result += " style=\"" + inline_style + "\"";
    }

    m_result += ">\n";

    bool is_void = void_elements.count(node.tagName) > 0;

    if (!is_void) {
        m_indent_level++;
        for (const auto& child : node.children) {
            // We need to avoid visiting the StyleNode again
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

    // Don't indent if the parent was an inline element, but for now, always indent.
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
    // The lexeme for GENERATOR_COMMENT is "-- comment text"
    // We need to strip the "--" part.
    std::string content = node.content;
    if (content.rfind("--", 0) == 0) {
        content = content.substr(2);
    }
    // trim leading space
    size_t first = content.find_first_not_of(" \t");
    if (std::string::npos != first) {
        content = content.substr(first);
    }

    m_result += "<!-- " + escapeHtml(content) + " -->\n";
}

void HtmlGenerator::visit(StyleNode& node) {
    // Style nodes are handled by their parent ElementNode to generate
    // inline styles or global style blocks. They don't generate output themselves.
}

void HtmlGenerator::visit(TemplateDefinitionNode& node) {
    // Template definitions are collected in a first pass and do not generate output directly.
}

} // namespace CHTL
