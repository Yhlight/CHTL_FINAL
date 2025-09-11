#include "CHTLGenerator.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <set>

namespace CHTL {

// Helper to convert an EvaluatedValue to a string
std::string ValueToString(const EvaluatedValue& val) {
    if (val.type == ValueType::String) {
        return std::get<std::string>(val.value);
    }
    if (val.type == ValueType::Number) {
        // Use stringstream to avoid trailing zeros
        std::stringstream ss;
        ss << std::get<double>(val.value);
        return ss.str() + val.unit;
    }
    return ""; // Should not happen in this simplified version
}

// Helper for HTML escaping
std::string escapeHTML(const std::string& data) {
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


std::string CHTLGenerator::Generate(const NodeList& ast) {
    m_output_body.str(""); // Clear the stream
    for (const auto& node : ast) {
        Visit(node);
    }
    return m_output_body.str();
}

void CHTLGenerator::Visit(const NodePtr& node) {
    if (!node) return;
    switch (node->GetType()) {
        case NodeType::Element:
            VisitElement(static_cast<const ElementNode*>(node.get()));
            break;
        case NodeType::Text:
            VisitText(static_cast<const TextNode*>(node.get()));
            break;
        case NodeType::Comment:
            VisitComment(static_cast<const CommentNode*>(node.get()));
            break;
        default:
            // For now, we ignore other node types
            break;
    }
}

void CHTLGenerator::VisitElement(const ElementNode* node) {
    static const std::set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };

    m_output_body << "<" << node->GetTagName();

    // Process properties (attributes)
    for (const auto& prop : node->GetProperties()) {
        EvaluatedValue val = EvaluateExpression(prop.value);
        m_output_body << " " << prop.name << "=\"" << escapeHTML(ValueToString(val)) << "\"";
    }

    m_output_body << ">";

    if (selfClosingTags.count(node->GetTagName()) == 0) {
        // Not a self-closing tag, so process children and add closing tag
        for (const auto& child : node->GetChildren()) {
            Visit(child);
        }
        m_output_body << "</" << node->GetTagName() << ">";
    }
}

void CHTLGenerator::VisitText(const TextNode* node) {
    m_output_body << escapeHTML(node->GetText());
}

void CHTLGenerator::VisitComment(const CommentNode* node) {
    if (node->GetCommentType() == CommentType::Generator) {
        m_output_body << "<!--" << escapeHTML(node->GetText()) << "-->";
    }
}

// --- Simplified Expression Evaluation ---

EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr) {
    if (!expr) return {ValueType::String, ""}; // Default to empty string if no expression

    switch (expr->GetType()) {
        case ExpressionNodeType::StringLiteral:
            return {ValueType::String, static_cast<const StringLiteralNode*>(expr.get())->GetValue()};
        case ExpressionNodeType::NumberLiteral:
            return {ValueType::Number, static_cast<const NumberLiteralNode*>(expr.get())->GetValue(), static_cast<const NumberLiteralNode*>(expr.get())->GetUnit()};
        default:
            // In this simplified generator, we don't support other expression types yet.
            // We can throw an error or return a default value.
            throw std::runtime_error("Unsupported expression type in simplified generator.");
    }
}

} // namespace CHTL
