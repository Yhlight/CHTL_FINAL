#include "CHTLGenerator.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>

namespace CHTL {

// --- Expression Evaluation Logic ---

EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr) {
    if (!expr) return {ValueType::String, ""};
    return VisitExpression(expr.get());
}

EvaluatedValue CHTLGenerator::VisitExpression(const ExpressionNode* expr) {
    switch (expr->GetType()) {
        case ExpressionNodeType::NumberLiteral:
            return VisitNumberLiteral(static_cast<const NumberLiteralNode*>(expr));
        case ExpressionNodeType::StringLiteral:
            return VisitStringLiteral(static_cast<const StringLiteralNode*>(expr));
        case ExpressionNodeType::BinaryOp:
            return VisitBinaryOp(static_cast<const BinaryOpNode*>(expr));
        case ExpressionNodeType::PropertyReference:
            return VisitPropertyReference(static_cast<const PropertyReferenceNode*>(expr));
        case ExpressionNodeType::TemplateUsage:
            return {ValueType::String, ""}; // Should not be evaluated
        default:
            throw std::runtime_error("Unknown expression node type.");
    }
}

EvaluatedValue CHTLGenerator::VisitNumberLiteral(const NumberLiteralNode* node) {
    return {ValueType::Number, node->GetValue(), node->GetUnit()};
}

EvaluatedValue CHTLGenerator::VisitStringLiteral(const StringLiteralNode* node) {
    return {ValueType::String, node->GetValue(), ""};
}

EvaluatedValue CHTLGenerator::VisitBinaryOp(const BinaryOpNode* node) {
    EvaluatedValue left = VisitExpression(node->GetLeft().get());
    EvaluatedValue right = VisitExpression(node->GetRight().get());

    if (left.type != ValueType::Number || right.type != ValueType::Number) {
        throw std::runtime_error("Arithmetic operations can only be performed on numbers.");
    }

    double left_val = std::get<double>(left.value);
    double right_val = std::get<double>(right.value);

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Incompatible units in expression.");
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_value = 0;
    switch (node->GetOperator()) {
        case TokenType::Plus: result_value = left_val + right_val; break;
        case TokenType::Minus: result_value = left_val - right_val; break;
        case TokenType::Star: result_value = left_val * right_val; break;
        case TokenType::Slash: if (right_val == 0) throw std::runtime_error("Division by zero."); result_value = left_val / right_val; break;
        case TokenType::Percent: result_value = fmod(left_val, right_val); break;
        case TokenType::DoubleStar: result_value = pow(left_val, right_val); break;
        default: throw std::runtime_error("Unsupported operator.");
    }
    return {ValueType::Number, result_value, result_unit};
}

const ElementNode* CHTLGenerator::FindElement(const NodePtr& searchRoot, const std::string& selector) {
    if (!searchRoot || searchRoot->GetType() != NodeType::Element) return nullptr;
    const auto* currentElement = static_cast<const ElementNode*>(searchRoot.get());
    if (selector[0] == '#') {
        for (const auto& attr : currentElement->GetAttributes()) {
            if (attr.name == "id" && attr.value == selector.substr(1)) return currentElement;
        }
    } else if (selector[0] == '.') {
         for (const auto& attr : currentElement->GetAttributes()) {
            if (attr.name == "class" && attr.value == selector.substr(1)) return currentElement;
        }
    } else {
         if (currentElement->GetTagName() == selector) return currentElement;
    }
    for (const auto& child : currentElement->GetChildren()) {
        const ElementNode* found = FindElement(child, selector);
        if (found) return found;
    }
    return nullptr;
}


EvaluatedValue CHTLGenerator::VisitPropertyReference(const PropertyReferenceNode* node) {
    const ElementNode* targetElement = FindElement(m_astRoot, node->GetSelector());
    if (!targetElement) throw std::runtime_error("Could not find element with selector: " + node->GetSelector());
    for (const auto& child : targetElement->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());
            for (const auto& prop : styleNode->GetProperties()) {
                if (prop.name == node->GetPropertyName()) return EvaluateExpression(prop.value);
            }
            for (const auto& rule : styleNode->GetRules()) {
                 for (const auto& prop : rule->GetProperties()) {
                    if (prop.name == node->GetPropertyName()) return EvaluateExpression(prop.value);
                }
            }
        }
    }
    throw std::runtime_error("Could not find property '" + node->GetPropertyName() + "' on element '" + node->GetSelector() + "'");
}


// --- Style and HTML Generation Logic ---

struct StyleProcessingResult {
    std::vector<Property> inline_styles;
    std::vector<Attribute> attributes_to_add;
    std::string primary_selector;
};

// Helper to format the final value into a stringstream
void WriteEvaluatedValue(std::stringstream& ss, const EvaluatedValue& val) {
    if (val.type == ValueType::Number) {
        ss << std::get<double>(val.value) << val.unit;
    } else {
        ss << std::get<std::string>(val.value);
    }
}

void CHTLGenerator::ProcessStyleNodes(const ElementNode* node, StyleProcessingResult& result) {
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());

            for (const auto& prop : styleNode->GetProperties()) {
                if (prop.name == "__TEMPLATE_USAGE__") {
                    const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());
                    auto it = m_template_repo.find(usageNode->GetTemplateName());
                    if (it != m_template_repo.end()) {
                        const auto* templateDef = it->second;
                        const auto* templateStyleNode = static_cast<const StyleNode*>(templateDef->GetContent().get());
                        for (const auto& templateProp : templateStyleNode->GetProperties()) {
                             result.inline_styles.push_back(templateProp);
                        }
                    }
                } else {
                    result.inline_styles.push_back(prop);
                }
            }

            for (const auto& rule : styleNode->GetRules()) {
                std::string selector = rule->GetSelector();
                if (selector[0] == '.' || selector[0] == '#') {
                    std::string attr_name = (selector[0] == '.') ? "class" : "id";
                    std::string attr_value = selector.substr(1);
                    auto it_attr = std::find_if(result.attributes_to_add.begin(), result.attributes_to_add.end(),
                                           [&](const Attribute& a){ return a.name == attr_name; });
                    if (it_attr == result.attributes_to_add.end()) {
                        result.attributes_to_add.push_back({attr_name, attr_value});
                    } else {
                        if (attr_name == "class") it_attr->value += " " + attr_value; else it_attr->value = attr_value;
                    }
                    if (result.primary_selector.empty()) result.primary_selector = selector;
                }
                if (selector[0] == '&' && !result.primary_selector.empty()) {
                    selector.replace(0, 1, result.primary_selector);
                }
                m_output_css << selector << " { ";
                for (const auto& prop : rule->GetProperties()) {
                    EvaluatedValue val = EvaluateExpression(prop.value);
                    m_output_css << prop.name << ": ";
                    WriteEvaluatedValue(m_output_css, val);
                    m_output_css << "; ";
                }
                m_output_css << "}\n";
            }
        }
    }
}

std::string CHTLGenerator::Generate(const NodeList& ast) {
    m_template_repo.clear();
    m_astRoot = nullptr;
    for (const auto& node : ast) {
        if (node->GetType() == NodeType::Element) {
            m_astRoot = node;
            break;
        }
    }
    for (const auto& node : ast) {
        if (node->GetType() == NodeType::TemplateDefinition) {
            Visit(node);
        }
    }
    m_output_body.str("");
    m_output_css.str("");
    for (const auto& node : ast) {
        if (node->GetType() != NodeType::TemplateDefinition) {
            Visit(node);
        }
    }
    std::string body = m_output_body.str();
    std::string css = m_output_css.str();
    if (css.empty()) return body;
    return "<style>\n" + css + "</style>\n" + body;
}

void CHTLGenerator::Visit(const NodePtr& node) {
    if (!node) return;
    switch (node->GetType()) {
        case NodeType::Element: VisitElement(static_cast<const ElementNode*>(node.get())); break;
        case NodeType::Text: VisitText(static_cast<const TextNode*>(node.get())); break;
        case NodeType::Comment: VisitComment(static_cast<const CommentNode*>(node.get())); break;
        case NodeType::TemplateDefinition: VisitTemplateDefinition(static_cast<const TemplateDefinitionNode*>(node.get())); break;
        case NodeType::Style: break;
        default: throw std::runtime_error("Unknown node type in generator.");
    }
}

void CHTLGenerator::VisitTemplateDefinition(const TemplateDefinitionNode* node) {
    m_template_repo[node->GetName()] = node;
}

void CHTLGenerator::VisitElement(const ElementNode* node) {
    StyleProcessingResult style_result;
    style_result.attributes_to_add = node->GetAttributes();
    ProcessStyleNodes(node, style_result);

    m_output_body << "<" << node->GetTagName();
    for (const auto& attr : style_result.attributes_to_add) {
        m_output_body << " " << attr.name << "=\"" << attr.value << "\"";
    }

    if (!style_result.inline_styles.empty()) {
        m_output_body << " style=\"";
        for (const auto& prop : style_result.inline_styles) {
            EvaluatedValue val = EvaluateExpression(prop.value);
            m_output_body << prop.name << ":";
            WriteEvaluatedValue(m_output_body, val);
            m_output_body << ";";
        }
        m_output_body << "\"";
    }

    m_output_body << ">";
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() != NodeType::Style) Visit(child);
    }
    m_output_body << "</" << node->GetTagName() << ">";
}

void CHTLGenerator::VisitText(const TextNode* node) {
    m_output_body << node->GetText();
}

void CHTLGenerator::VisitComment(const CommentNode* node) {
    if (node->GetCommentType() == CommentType::Generator) {
        m_output_body << "<!--" << node->GetText() << "-->";
    }
}

} // namespace CHTL
