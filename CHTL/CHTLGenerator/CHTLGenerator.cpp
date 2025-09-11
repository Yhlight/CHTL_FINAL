#include "CHTLGenerator.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <cmath>
#include <algorithm>
#include <map>

// This file is being restored to the state at the end of the "Template System" phase.

namespace CHTL {

// --- Helper Functions ---

std::string ValueToString(const EvaluatedValue& val) {
    if (val.type == ValueType::String) {
        return std::get<std::string>(val.value);
    }
    if (val.type == ValueType::Number) {
        std::stringstream ss;
        ss << std::get<double>(val.value);
        return ss.str() + val.unit;
    }
     if (val.type == ValueType::Bool) {
        return std::get<bool>(val.value) ? "true" : "false";
    }
    return "";
}

std::string CHTLGenerator::escapeHTML(const std::string& data) {
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


// --- Main Generation Logic ---

std::string CHTLGenerator::Generate(const NodeList& ast) {
    m_output_body.str("");
    m_output_css.str("");
    m_template_repo.clear();

    for (const auto& node : ast) {
        if (node->GetType() == NodeType::TemplateDefinition) {
            VisitTemplateDefinition(static_cast<const TemplateDefinitionNode*>(node.get()));
        }
    }

    for (const auto& node : ast) {
        if (node->GetType() != NodeType::TemplateDefinition) {
            Visit(node);
        }
    }

    std::string css = m_output_css.str();
    std::string body = m_output_body.str();

    if (css.empty()) {
        return body;
    }

    return "<style>\n" + css + "</style>\n" + body;
}

void CHTLGenerator::Visit(const NodePtr& node) {
    if (!node) return;
    switch (node->GetType()) {
        case NodeType::Element: VisitElement(static_cast<const ElementNode*>(node.get())); break;
        case NodeType::Text: VisitText(static_cast<const TextNode*>(node.get())); break;
        case NodeType::Comment: VisitComment(static_cast<const CommentNode*>(node.get())); break;
        case NodeType::ElementTemplateUsage: VisitElementTemplateUsage(static_cast<const ElementTemplateUsageNode*>(node.get())); break;
        case NodeType::TemplateDefinition: break;
        case NodeType::Style: break;
        default: break;
    }
}

void CHTLGenerator::VisitTemplateDefinition(const TemplateDefinitionNode* node) {
    if (m_template_repo.count(node->GetName())) {
        throw std::runtime_error("Template with name '" + node->GetName() + "' is already defined.");
    }
    m_template_repo[node->GetName()] = node;
}

void CHTLGenerator::VisitElementTemplateUsage(const ElementTemplateUsageNode* node) {
    auto it = m_template_repo.find(node->GetTemplateName());
    if (it == m_template_repo.end()) {
        throw std::runtime_error("Undefined element template used: @" + node->GetTemplateName());
    }
    const auto* templateDef = it->second;
    if (templateDef->GetTemplateType() != TemplateType::Element) {
        throw std::runtime_error("Mismatched template type usage for @" + node->GetTemplateName() + ". Expected @Element.");
    }

    for (const auto& contentNode : templateDef->GetContent()) {
        Visit(contentNode);
    }
}

std::vector<Property> CHTLGenerator::ExpandStyleTemplate(const std::string& templateName, const std::shared_ptr<StyleNode>& specialization) {
    auto it = m_template_repo.find(templateName);
    if (it == m_template_repo.end()) throw std::runtime_error("Undefined style template used: " + templateName);

    const auto* templateDef = it->second;
    if (templateDef->GetTemplateType() != TemplateType::Style && templateDef->GetTemplateType() != TemplateType::Var) {
        throw std::runtime_error("Template '" + templateName + "' is not a @Style or @Var template.");
    }

    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());
    std::map<std::string, Property> final_properties;

    // First, expand any inherited templates
    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == "__TEMPLATE_USAGE__") {
            const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());
            // Pass the specialization from the inherited usage node
            std::vector<Property> inherited_props = ExpandStyleTemplate(usageNode->GetTemplateName(), usageNode->GetSpecialization());
            for (const auto& inherited_prop : inherited_props) {
                if (final_properties.find(inherited_prop.name) == final_properties.end()) {
                    final_properties[inherited_prop.name] = inherited_prop;
                }
            }
        } else {
            final_properties[prop.name] = prop;
        }
    }

    std::set<std::string> deleted_items;
    // Now, apply specializations from the current usage site and check for deletions
    if (specialization) {
        for (const auto& spec_prop : specialization->GetProperties()) {
            if (spec_prop.name == "__DELETE__") {
                auto valueNode = std::dynamic_pointer_cast<StringLiteralNode>(spec_prop.value);
                if (valueNode) {
                    std::stringstream ss(valueNode->GetValue());
                    std::string item;
                    while(std::getline(ss, item, ',')) {
                        deleted_items.insert(item);
                    }
                }
            } else {
                final_properties[spec_prop.name] = spec_prop;
            }
        }
    }

    std::vector<Property> result;
    for (auto const& [name, prop] : final_properties) {
        if (deleted_items.count(name)) continue;
        if (prop.value == nullptr) {
            if (templateDef->IsCustom()) {
                 throw std::runtime_error("Valueless property '" + name + "' from custom template '" + templateName + "' was not provided a value.");
            }
        } else {
            result.push_back(prop);
        }
    }

    return result;
}


void CHTLGenerator::VisitElement(const ElementNode* node) {
    static const std::set<std::string> selfClosingTags = { "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr" };
    m_output_body << "<" << node->GetTagName();

    std::stringstream inline_style_stream;
    std::string primary_selector;
    std::vector<Property> inline_properties;

    for (const auto& prop : node->GetProperties()) {
        if (prop.name == "id") primary_selector = "#" + ValueToString(EvaluateExpression(prop.value, node));
        else if (prop.name == "class" && primary_selector.empty()) primary_selector = "." + ValueToString(EvaluateExpression(prop.value, node));
        EvaluatedValue val = EvaluateExpression(prop.value, node);
        m_output_body << " " << prop.name << "=\"" << escapeHTML(ValueToString(val)) << "\"";
    }

    for (const auto& child : node->GetChildren()) {
        if (child->GetType() == NodeType::Style) {
            const auto* styleNode = static_cast<const StyleNode*>(child.get());

            for (const auto& prop : styleNode->GetProperties()) {
                if (prop.name == "__TEMPLATE_USAGE__") {
                    const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());
                    std::vector<Property> expanded_props = ExpandStyleTemplate(usageNode->GetTemplateName(), usageNode->GetSpecialization());
                    inline_properties.insert(inline_properties.end(), expanded_props.begin(), expanded_props.end());
                } else {
                    inline_properties.push_back(prop);
                }
            }

            for (const auto& rule : styleNode->GetRules()) {
                VisitStyleRule(rule.get(), node, primary_selector);
            }
        }
    }

    std::map<std::string, Property> final_inline_props;
    for(const auto& prop : inline_properties) {
        final_inline_props[prop.name] = prop;
    }

    for(const auto& pair : final_inline_props) {
        EvaluatedValue val = EvaluateExpression(pair.second.value, node);
        inline_style_stream << pair.second.name << ":" << ValueToString(val) << ";";
    }

    std::string inline_style = inline_style_stream.str();
    if (!inline_style.empty()) {
        m_output_body << " style=\"" << escapeHTML(inline_style) << "\"";
    }

    m_output_body << ">";

    if (selfClosingTags.count(node->GetTagName()) == 0) {
        for (const auto& child : node->GetChildren()) {
            if (child->GetType() != NodeType::Style) Visit(child);
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

void CHTLGenerator::VisitStyleRule(const StyleRuleNode* node, const ElementNode* context, const std::string& primary_selector) {
    std::string final_selector = node->GetSelector();
    if (final_selector.rfind('&', 0) == 0) {
        if (primary_selector.empty()) throw std::runtime_error("Cannot use '&' selector on an element with no class or id.");
        final_selector.replace(0, 1, primary_selector);
    }

    m_output_css << final_selector << " {\n";
    for (const auto& prop : node->GetProperties()) {
        EvaluatedValue val = EvaluateExpression(prop.value, context);
        m_output_css << "  " << prop.name << ": " << ValueToString(val) << ";\n";
    }
    m_output_css << "}\n";
}


// --- Full Expression Evaluation ---

EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* context) {
    if (!expr) return {ValueType::String, ""};

    switch (expr->GetType()) {
        case ExpressionNodeType::StringLiteral:
            return {ValueType::String, static_cast<const StringLiteralNode*>(expr.get())->GetValue()};
        case ExpressionNodeType::NumberLiteral:
            return {ValueType::Number, static_cast<const NumberLiteralNode*>(expr.get())->GetValue(), static_cast<const NumberLiteralNode*>(expr.get())->GetUnit()};
        case ExpressionNodeType::BinaryOp:
            return VisitBinaryOp(static_cast<const BinaryOpNode*>(expr.get()), context);
        case ExpressionNodeType::Conditional:
            return VisitConditionalExpr(static_cast<const ConditionalExprNode*>(expr.get()), context);
        case ExpressionNodeType::TemplateUsage:
            throw std::runtime_error("Template usages should be expanded before expression evaluation.");
        case ExpressionNodeType::VariableUsage:
            return VisitVariableUsage(static_cast<const VariableUsageNode*>(expr.get()), context);
        default:
            throw std::runtime_error("Unsupported expression type in generator.");
    }
}

EvaluatedValue CHTLGenerator::VisitVariableUsage(const VariableUsageNode* node, const ElementNode* context) {
    // Check if the variable is being used with a specialization
    if (node->IsSpecialized()) {
        // In a specialization like `ThemeColor(primary = "blue")`, we just evaluate the specialized expression.
        // This logic currently only supports one specialization at a time, which matches the parser.
        auto const& [varName, varValue] = *node->GetSpecializations().begin();
        return EvaluateExpression(varValue, context);
    }

    // This is for simple usage like `ThemeColor(primary)`
    auto it = m_template_repo.find(node->GetGroupName());
    if (it == m_template_repo.end()) throw std::runtime_error("Undefined variable group used: " + node->GetGroupName());

    const auto* templateDef = it->second;
    if (templateDef->GetTemplateType() != TemplateType::Var) throw std::runtime_error("Template is not a variable group: " + node->GetGroupName());

    // The content of a @Var template is a StyleNode
    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());

    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == node->GetVariableName()) {
            // Recursively evaluate the variable's value. Pass nullptr for context because vars can't self-reference.
            return EvaluateExpression(prop.value, nullptr);
        }
    }

    throw std::runtime_error("Undefined variable '" + node->GetVariableName() + "' in group '" + node->GetGroupName() + "'");
}

EvaluatedValue CHTLGenerator::VisitBinaryOp(const BinaryOpNode* node, const ElementNode* context) {
    EvaluatedValue left = EvaluateExpression(node->GetLeft(), context);
    EvaluatedValue right = EvaluateExpression(node->GetRight(), context);
    TokenType op = node->GetOperator();

    if (op == TokenType::LogicalAnd || op == TokenType::LogicalOr) {
        bool l_bool = (left.type == ValueType::Bool) ? std::get<bool>(left.value) : (left.type == ValueType::Number && std::get<double>(left.value) != 0);
        bool r_bool = (right.type == ValueType::Bool) ? std::get<bool>(right.value) : (right.type == ValueType::Number && std::get<double>(right.value) != 0);
        return {ValueType::Bool, (op == TokenType::LogicalAnd ? (l_bool && r_bool) : (l_bool || r_bool))};
    }

    if (op == TokenType::Plus && (left.type == ValueType::String || right.type == ValueType::String)) {
        return {ValueType::String, ValueToString(left) + ValueToString(right)};
    }

    if (left.type != ValueType::Number || right.type != ValueType::Number) {
        throw std::runtime_error("Arithmetic and comparison operators require numeric operands.");
    }

    double l_val = std::get<double>(left.value);
    double r_val = std::get<double>(right.value);

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
        return {ValueType::Bool, result};
    }

    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Cannot perform arithmetic on numbers with incompatible units.");
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_val = 0;
    switch (op) {
        case TokenType::Plus: result_val = l_val + r_val; break;
        case TokenType::Minus: result_val = l_val - r_val; break;
        case TokenType::Star: result_val = l_val * r_val; break;
        case TokenType::Slash: if (r_val == 0) throw std::runtime_error("Division by zero."); result_val = l_val / r_val; break;
        case TokenType::Percent: result_val = fmod(l_val, r_val); break;
        case TokenType::DoubleStar: result_val = pow(l_val, r_val); break;
        default: throw std::runtime_error("Unsupported binary operator.");
    }
    return {ValueType::Number, result_val, result_unit};
}

EvaluatedValue CHTLGenerator::VisitConditionalExpr(const ConditionalExprNode* node, const ElementNode* context) {
    EvaluatedValue condition = EvaluateExpression(node->GetCondition(), context);
    bool condition_bool = (condition.type == ValueType::Bool) ? std::get<bool>(condition.value) : (condition.type == ValueType::Number && std::get<double>(condition.value) != 0);

    if (condition_bool) {
        return EvaluateExpression(node->GetTrueBranch(), context);
    } else {
        if (node->GetFalseBranch()) {
            return EvaluateExpression(node->GetFalseBranch(), context);
        }
        return {ValueType::String, ""};
    }
}

} // namespace CHTL
