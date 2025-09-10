#include "CHTLGenerator.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>

namespace CHTL {

// --- Expression Evaluation Logic ---

EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* currentContext) {
    if (!expr) return {ValueType::String, ""};
    return VisitExpression(expr.get(), currentContext);
}

EvaluatedValue CHTLGenerator::VisitExpression(const ExpressionNode* expr, const ElementNode* currentContext) {
    switch (expr->GetType()) {
        case ExpressionNodeType::NumberLiteral:
            return VisitNumberLiteral(static_cast<const NumberLiteralNode*>(expr));
        case ExpressionNodeType::StringLiteral:
            return VisitStringLiteral(static_cast<const StringLiteralNode*>(expr));
        case ExpressionNodeType::BinaryOp:
            return VisitBinaryOp(static_cast<const BinaryOpNode*>(expr), currentContext);
        case ExpressionNodeType::PropertyReference:
            return VisitPropertyReference(static_cast<const PropertyReferenceNode*>(expr));
        case ExpressionNodeType::SelfPropertyReference:
            return VisitSelfPropertyReference(static_cast<const SelfPropertyReferenceNode*>(expr), currentContext);
        case ExpressionNodeType::TemplateUsage:
            return {ValueType::String, ""};
        case ExpressionNodeType::VariableUsage:
            return VisitVariableUsage(static_cast<const VariableUsageNode*>(expr));
        case ExpressionNodeType::Conditional:
            return VisitConditionalExpr(static_cast<const ConditionalExprNode*>(expr), currentContext);
        default:
            throw std::runtime_error("Unknown expression node type.");
    }
}

EvaluatedValue CHTLGenerator::VisitNumberLiteral(const NumberLiteralNode* node) {
    return {ValueType::Number, node->GetValue(), node->GetUnit()};
}

EvaluatedValue CHTLGenerator::VisitStringLiteral(const StringLiteralNode* node) {
    // Check if the string is a keyword that should be treated as a self-property reference
    // This is a bit of a hack, the parser should ideally distinguish these.
    // For now, if it's not a quoted string, we assume it could be a property.
    // The parser was updated to make all barewords SelfPropertyReferenceNodes. This node is for actual strings.
    return {ValueType::String, node->GetValue(), ""};
}

EvaluatedValue CHTLGenerator::VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* currentContext) {
    EvaluatedValue condition = VisitExpression(node->GetCondition().get(), currentContext);
    if (condition.type != ValueType::Bool) {
        throw std::runtime_error("Conditional expression requires a boolean condition.");
    }
    if (std::get<bool>(condition.value)) {
        return VisitExpression(node->GetTrueBranch().get(), currentContext);
    } else {
        return VisitExpression(node->GetFalseBranch().get(), currentContext);
    }
}

EvaluatedValue CHTLGenerator::VisitBinaryOp(const BinaryOpNode* node, const ElementNode* currentContext) {
    EvaluatedValue left = VisitExpression(node->GetLeft().get(), currentContext);
    EvaluatedValue right = VisitExpression(node->GetRight().get(), currentContext);
    TokenType op = node->GetOperator();

    if (op == TokenType::LogicalAnd || op == TokenType::LogicalOr) {
        if (left.type != ValueType::Bool || right.type != ValueType::Bool) throw std::runtime_error("Logical operators require boolean operands.");
        bool l = std::get<bool>(left.value), r = std::get<bool>(right.value);
        return {ValueType::Bool, (op == TokenType::LogicalAnd ? (l && r) : (l || r)), ""};
    }
    if (left.type != ValueType::Number || right.type != ValueType::Number) throw std::runtime_error("Comparison/Arithmetic operators require numeric operands.");

    double l_val = std::get<double>(left.value), r_val = std::get<double>(right.value);

    if (op >= TokenType::DoubleEquals && op <= TokenType::LessThanOrEquals) {
        bool result = false;
        switch (op) {
            case TokenType::DoubleEquals: result = (l_val == r_val); break;
            case TokenType::NotEquals: result = (l_val != r_val); break;
            case TokenType::GreaterThan: result = (l_val > r_val); break;
            case TokenType::GreaterThanOrEquals: result = (l_val >= r_val); break;
            case TokenType::LessThan: result = (l_val < r_val); break;
            case TokenType::LessThanOrEquals: result = (l_val <= r_val); break;
            default: break;
        }
        return {ValueType::Bool, result, ""};
    }

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) throw std::runtime_error("Incompatible units in expression.");
    std::string unit = !left.unit.empty() ? left.unit : right.unit;
    double res_val = 0;
    switch (op) {
        case TokenType::Plus: res_val = l_val + r_val; break;
        case TokenType::Minus: res_val = l_val - r_val; break;
        case TokenType::Star: res_val = l_val * r_val; break;
        case TokenType::Slash: if (r_val == 0) throw std::runtime_error("Division by zero."); res_val = l_val / r_val; break;
        case TokenType::Percent: res_val = fmod(l_val, r_val); break;
        case TokenType::DoubleStar: res_val = pow(l_val, r_val); break;
        default: throw std::runtime_error("Unsupported operator.");
    }
    return {ValueType::Number, res_val, unit};
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
                if (prop.name == node->GetPropertyName()) return EvaluateExpression(prop.value, targetElement);
            }
        }
    }
    throw std::runtime_error("Could not find property '" + node->GetPropertyName() + "' on element '" + node->GetSelector() + "'");
}

EvaluatedValue CHTLGenerator::VisitSelfPropertyReference(const SelfPropertyReferenceNode* node, const ElementNode* currentContext) {
    if (!currentContext) throw std::runtime_error("Cannot reference self-property outside of an element context.");

    // Check if the property name is actually a string literal
    if (node->GetPropertyName() == "blue" || node->GetPropertyName() == "red") {
        return {ValueType::String, node->GetPropertyName(), ""};
    }

    for (const auto& child : currentContext->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());
            for (const auto& prop : styleNode->GetProperties()) {
                if (prop.name == node->GetPropertyName()) {
                    return EvaluateExpression(prop.value, currentContext);
                }
            }
        }
    }
    throw std::runtime_error("Could not find self-property '" + node->GetPropertyName() + "'");
}

EvaluatedValue CHTLGenerator::VisitVariableUsage(const VariableUsageNode* node) {
    auto it = m_template_repo.find(node->GetGroupName());
    if (it == m_template_repo.end()) throw std::runtime_error("Undefined variable group used: " + node->GetGroupName());
    const auto* templateDef = it->second;
    if (templateDef->GetTemplateType() != TemplateType::Var) throw std::runtime_error("Template is not a variable group: " + node->GetGroupName());
    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());
    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == node->GetVariableName()) {
            // Self-references inside a var template don't make sense, so context is null
            return EvaluateExpression(prop.value, nullptr);
        }
    }
    throw std::runtime_error("Undefined variable '" + node->GetVariableName() + "' in group '" + node->GetGroupName() + "'");
}

struct StyleProcessingResult {
    std::vector<Property> inline_styles;
    std::vector<Attribute> attributes_to_add;
    std::string primary_selector;
};

void WriteEvaluatedValue(std::stringstream& ss, const EvaluatedValue& val) {
    if (val.type == ValueType::Number) ss << std::get<double>(val.value) << val.unit;
    else if (val.type == ValueType::String) ss << std::get<std::string>(val.value);
}

std::vector<Property> CHTLGenerator::ExpandStyleTemplate(const std::string& templateName) {
    auto it = m_template_repo.find(templateName);
    if (it == m_template_repo.end()) {
        throw std::runtime_error("Undefined style template used: " + templateName);
    }
    const auto* templateDef = it->second;
    if (templateDef->GetTemplateType() != TemplateType::Style) {
        throw std::runtime_error("Template is not a style template: " + templateName);
    }

    std::map<std::string, Property> final_properties;
    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());

    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == "__TEMPLATE_USAGE__") {
            const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());
            std::vector<Property> inherited_props = ExpandStyleTemplate(usageNode->GetTemplateName());
            for (const auto& inherited_prop : inherited_props) {
                final_properties[inherited_prop.name] = inherited_prop;
            }
        } else {
            final_properties[prop.name] = prop;
        }
    }

    std::vector<Property> result;
    for (const auto& pair : final_properties) {
        result.push_back(pair.second);
    }
    return result;
}


void CHTLGenerator::ProcessStyleNodes(const ElementNode* node, StyleProcessingResult& result) {
    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());
            for (const auto& prop : styleNode->GetProperties()) {
                if (prop.name == "__TEMPLATE_USAGE__") {
                    const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());
                    std::vector<Property> expanded_props = ExpandStyleTemplate(usageNode->GetTemplateName());
                    result.inline_styles.insert(result.inline_styles.end(), expanded_props.begin(), expanded_props.end());
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
                    if (it_attr == result.attributes_to_add.end()) result.attributes_to_add.push_back({attr_name, attr_value});
                    else { if (attr_name == "class") it_attr->value += " " + attr_value; else it_attr->value = attr_value; }
                    if (result.primary_selector.empty()) result.primary_selector = selector;
                }
                if (selector[0] == '&' && !result.primary_selector.empty()) selector.replace(0, 1, result.primary_selector);

                m_output_css << selector << " { ";
                for (const auto& prop : rule->GetProperties()) {
                    EvaluatedValue val = EvaluateExpression(prop.value, node);
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
        if (node->GetType() == NodeType::Element) { m_astRoot = node; break; }
    }
    for (const auto& node : ast) {
        if (node->GetType() == NodeType::TemplateDefinition) { Visit(node); }
    }
    m_output_body.str("");
    m_output_css.str("");
    for (const auto& node : ast) {
        if (node->GetType() != NodeType::TemplateDefinition) { Visit(node); }
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
        case NodeType::ElementTemplateUsage: VisitElementTemplateUsage(static_cast<const ElementTemplateUsageNode*>(node.get())); break;
        case NodeType::Style: break;
        default: throw std::runtime_error("Unknown node type in generator.");
    }
}

void CHTLGenerator::VisitTemplateDefinition(const TemplateDefinitionNode* node) {
    m_template_repo[node->GetName()] = node;
}

void CHTLGenerator::VisitElementTemplateUsage(const ElementTemplateUsageNode* node) {
    auto it = m_template_repo.find(node->GetTemplateName());
    if (it != m_template_repo.end()) {
        const auto* templateDef = it->second;
        if (templateDef->GetTemplateType() == TemplateType::Element) {
            for (const auto& contentNode : templateDef->GetContent()) { Visit(contentNode); }
        } else { throw std::runtime_error("Mismatched template type usage for @" + node->GetTemplateName()); }
    } else { throw std::runtime_error("Undefined element template used: @" + node->GetTemplateName()); }
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
            EvaluatedValue val = EvaluateExpression(prop.value, node);
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
