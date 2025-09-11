#include "CHTLGenerator.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/LiteralExpressionNode.h"
#include <vector>

namespace CHTL {

CHTLGenerator::CHTLGenerator(NodePtr rootNode, bool default_struct)
    : m_rootNode(std::move(rootNode)), m_default_struct(default_struct) {}

std::string CHTLGenerator::Generate() {
    if (m_rootNode) {
        visit(m_rootNode);
    }
    std::string final_output = m_output.str();

    // Inject global CSS
    std::string global_styles = m_global_css.str();
    if (!global_styles.empty()) {
        std::string style_tag = "<style>" + global_styles + "</style>";
        size_t head_pos = final_output.find("</head>");
        if (head_pos != std::string::npos) {
            final_output.insert(head_pos, style_tag);
        } else {
            final_output = style_tag + final_output;
        }
    }

    // Inject global JS
    std::string global_js = m_global_js.str();
    if (!global_js.empty()) {
        std::string script_tag = "<script>" + global_js + "</script>";
        size_t body_pos = final_output.find("</body>");
        if (body_pos != std::string::npos) {
            final_output.insert(body_pos, script_tag);
        } else {
            final_output += script_tag;
        }
    }

    // Add default structure if requested and if an <html> tag isn't already present
    if (m_default_struct) {
        bool html_tag_exists = false;
        if (auto element = std::dynamic_pointer_cast<ElementNode>(m_rootNode)) {
            if (element->tagName == "html") {
                html_tag_exists = true;
            }
        }
        if (!html_tag_exists) {
            final_output = "<!DOCTYPE html>\n<html>\n<head></head>\n<body>\n" + final_output + "\n</body>\n</html>";
        }
    }


    return final_output;
}

std::string CHTLGenerator::generateExpression(const ExpressionPtr& node, bool is_sub_expression) {
    if (auto literal = std::dynamic_pointer_cast<LiteralExpressionNode>(node)) {
        return literal->token.literal;
    }
    return "INVALID_EXPRESSION";
}

void CHTLGenerator::visit(const NodePtr& node) {
    if (!node) return;
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitText(text);
    }
}

void CHTLGenerator::visitElement(const std::shared_ptr<ElementNode>& node) {
    m_output << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        m_output << " ";
        visitAttribute(attr);
    }

    CSSPropertyList all_inline_properties;
    NodeList other_children;

    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            all_inline_properties.insert(all_inline_properties.end(), styleNode->inline_properties.begin(), styleNode->inline_properties.end());
        } else if (auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(child)) {
            m_global_js << scriptNode->content << std::endl;
        }
        else {
            other_children.push_back(child);
        }
    }

    if (!all_inline_properties.empty()) {
        m_output << " style=\"";
        for (const auto& prop : all_inline_properties) {
            m_output << prop->key << ":" << generateExpression(prop->value) << ";";
        }
        m_output << "\"";
    }

    m_output << ">";
    for (const auto& child : other_children) {
        visit(child);
    }
    m_output << "</" << node->tagName << ">";
}

void CHTLGenerator::visitText(const std::shared_ptr<TextNode>& node) {
    m_output << node->text;
}

void CHTLGenerator::visitAttribute(const std::shared_ptr<AttributeNode>& node) {
    m_output << node->key << "=\"" << node->value << "\"";
}

}
