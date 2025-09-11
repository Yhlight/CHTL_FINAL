#include "CHTLGenerator.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/ElementModificationNode.h"
#include "CHTLNode/ElementDeleteNode.h"
#include "CHTLNode/ElementInsertNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StyleRuleNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/SelfPropertyReferenceNode.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include "CHTLCompiler/CompilationContext.h"

namespace CHTL {

CHTLGenerator::CHTLGenerator(const CompilationContext& context) : m_context(context) {}

// --- Helper Functions ---
std::string ValueToString(const EvaluatedValue& val) { if (val.type == ValueType::String) return std::get<std::string>(val.value); if (val.type == ValueType::Number) { std::stringstream ss; ss << std::get<double>(val.value); return ss.str() + val.unit; } if (val.type == ValueType::Bool) return std::get<bool>(val.value) ? "true" : "false"; return ""; }
std::string CHTLGenerator::escapeHTML(const std::string& data) { std::string buffer; buffer.reserve(data.size()); for(size_t pos = 0; pos != data.size(); ++pos) { switch(data[pos]) { case '&': buffer.append("&amp;"); break; case '\"': buffer.append("&quot;"); break; case '\'': buffer.append("&apos;"); break; case '<': buffer.append("&lt;"); break; case '>': buffer.append("&gt;"); break; default: buffer.append(&data[pos], 1); break; } } return buffer; }


// --- Main Generation Logic ---

std::string CHTLGenerator::Generate(const NodeList& ast) {
    m_output_body.str(""); m_output_css.str(""); m_output_js.str(""); m_astRoot = ast.empty() ? nullptr : ast.front();
    // The template population is now handled by the parser and context.
    // The generator just needs to visit the nodes of the main file.
    for (const auto& node : ast) {
        Visit(node);
    }
    std::string css = m_output_css.str();
    std::string js = m_output_js.str();
    std::string body = m_output_body.str();

    std::string final_output;
    if (!css.empty()) {
        final_output += "<style>\n" + css + "</style>\n";
    }
    final_output += body;
    if (!js.empty()) {
        final_output += "\n<script>\n" + js + "</script>\n";
    }

    return final_output;
}

void CHTLGenerator::Visit(const NodePtr& node) {
    if (!node) return;
    switch (node->GetType()) {
        case NodeType::Element: VisitElement(static_cast<const ElementNode*>(node.get())); break;
        case NodeType::Text: VisitText(static_cast<const TextNode*>(node.get())); break;
        case NodeType::Comment: VisitComment(static_cast<const CommentNode*>(node.get())); break;
        case NodeType::ElementTemplateUsage: VisitElementTemplateUsage(static_cast<const ElementTemplateUsageNode*>(node.get())); break;
        case NodeType::Origin: VisitOrigin(static_cast<const OriginNode*>(node.get())); break;
        // Template definitions and imports are handled by the parser/compiler, not generated directly.
        case NodeType::TemplateDefinition:
        case NodeType::Import:
            break;
        default: break;
    }
}

void CHTLGenerator::VisitTemplateDefinition(const TemplateDefinitionNode* node) {
    // This function is now effectively a no-op in the generator,
    // as template definitions are collected by the parser into the context.
    // It's kept for conceptual clarity but could be removed.
}


// --- Element Specialization Logic ---

NodePtr CHTLGenerator::FindNodeBySelector(NodeList& nodes, const std::string& selector) {
    std::string tagName = selector;
    int index = 0;
    size_t bracketPos = selector.find('[');
    if (bracketPos != std::string::npos) {
        tagName = selector.substr(0, bracketPos);
        index = std::stoi(selector.substr(bracketPos + 1));
    }

    int count = 0;
    for (const auto& node : nodes) {
        if (node->GetType() == NodeType::Element) {
            auto element = static_cast<ElementNode*>(node.get());
            if (element->GetTagName() == tagName) {
                if (count == index) {
                    return node;
                }
                count++;
            }
        }
    }
    return nullptr;
}


void CHTLGenerator::ApplyModification(NodeList& nodes, const ElementModificationNode* mod) {
    auto targetNode = FindNodeBySelector(nodes, mod->GetSelector());
    if (!targetNode || targetNode->GetType() != NodeType::Element) return;
    auto targetElement = std::static_pointer_cast<ElementNode>(targetNode);
    targetElement->AddChild(mod->GetStyle()->Clone());
}

void CHTLGenerator::ApplyDeletion(NodeList& nodes, const ElementDeleteNode* del) {
    for (const auto& selector : del->GetSelectors()) {
        nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [&](const NodePtr& node) {
            if (node->GetType() == NodeType::Element) {
                return static_cast<ElementNode*>(node.get())->GetTagName() == selector;
            }
            return false;
        }), nodes.end());
    }
}

void CHTLGenerator::ApplyInsertion(NodeList& nodes, const ElementInsertNode* ins) {
    if (ins->GetPosition() == InsertPosition::AtBottom) {
        for(const auto& nodeToInsert : ins->GetNodesToInsert()) {
            nodes.push_back(nodeToInsert->Clone());
        }
    }
}

void CHTLGenerator::VisitElementTemplateUsage(const ElementTemplateUsageNode* node) {
    std::string full_name = node->GetFrom();
    if (!full_name.empty()) {
        full_name += ".";
    }
    full_name += node->GetTemplateName();

    const auto* templateDef = m_context.GetTemplate(full_name);
    if (!templateDef) throw std::runtime_error("Undefined element template used: @" + full_name);

    if (templateDef->GetTemplateType() != TemplateType::Element) throw std::runtime_error("Mismatched template type.");

    NodeList clonedContent;
    for (const auto& contentNode : templateDef->GetContent()) {
        clonedContent.push_back(contentNode->Clone());
    }

    if (node->IsSpecialized()) {
        for(const auto& instruction : node->GetInstructions()) {
            switch(instruction->GetType()) {
                case NodeType::ElementModification:
                    ApplyModification(clonedContent, static_cast<const ElementModificationNode*>(instruction.get()));
                    break;
                case NodeType::ElementDeletion:
                    ApplyDeletion(clonedContent, static_cast<const ElementDeleteNode*>(instruction.get()));
                    break;
                case NodeType::ElementInsertion:
                    ApplyInsertion(clonedContent, static_cast<const ElementInsertNode*>(instruction.get()));
                    break;
                default: break;
            }
        }
    }

    for (const auto& contentNode : clonedContent) {
        Visit(contentNode);
    }
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

                    std::string full_name = usageNode->GetFrom();
                    if (!full_name.empty()) {
                        full_name += ".";
                    }
                    full_name += usageNode->GetTemplateName();

                    std::vector<Property> expanded_props = ExpandStyleTemplate(full_name, usageNode->GetSpecialization());
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
    for(const auto& prop : inline_properties) final_inline_props[prop.name] = prop;
    for(const auto& pair : final_inline_props) {
        EvaluatedValue val = EvaluateExpression(pair.second.value, node);
        inline_style_stream << pair.second.name << ":" << ValueToString(val) << ";";
    }
    std::string inline_style = inline_style_stream.str();
    if (!inline_style.empty()) m_output_body << " style=\"" << escapeHTML(inline_style) << "\"";
    m_output_body << ">";
    if (selfClosingTags.count(node->GetTagName()) == 0) {
        for (const auto& child : node->GetChildren()) {
            if (child->GetType() != NodeType::Style) Visit(child);
        }
        m_output_body << "</" << node->GetTagName() << ">";
    }
}
void CHTLGenerator::VisitText(const TextNode* node) { m_output_body << escapeHTML(node->GetText()); }
void CHTLGenerator::VisitComment(const CommentNode* node) { if (node->GetCommentType() == CommentType::Generator) m_output_body << "<!--" << escapeHTML(node->GetText()) << "-->"; }
void CHTLGenerator::VisitStyle(const StyleNode* node, const ElementNode* context) {
    for (const auto& rule : node->GetRules()) {
        VisitStyleRule(rule.get(), context, "");
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
std::vector<Property> CHTLGenerator::ExpandStyleTemplate(const std::string& templateName, const std::shared_ptr<StyleNode>& specialization) {
    const auto* templateDef = m_context.GetTemplate(templateName);
    if (!templateDef) throw std::runtime_error("Undefined style template used: " + templateName);

    if (templateDef->GetTemplateType() != TemplateType::Style && templateDef->GetTemplateType() != TemplateType::Var) throw std::runtime_error("Template '" + templateName + "' is not a @Style or @Var template.");
    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());
    std::map<std::string, Property> final_properties;
    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == "__TEMPLATE_USAGE__") {
            const auto* usageNode = static_cast<const TemplateUsageNode*>(prop.value.get());

            std::string full_name = usageNode->GetFrom();
            if (!full_name.empty()) {
                full_name += ".";
            }
            full_name += usageNode->GetTemplateName();

            std::vector<Property> inherited_props = ExpandStyleTemplate(full_name, usageNode->GetSpecialization());
            for (const auto& inherited_prop : inherited_props) {
                if (final_properties.find(inherited_prop.name) == final_properties.end()) final_properties[inherited_prop.name] = inherited_prop;
            }
        } else { final_properties[prop.name] = prop; }
    }
    std::set<std::string> deleted_items;
    if (specialization) {
        for (const auto& spec_prop : specialization->GetProperties()) {
            if (spec_prop.name == "__DELETE__") {
                auto valueNode = std::dynamic_pointer_cast<StringLiteralNode>(spec_prop.value);
                if (valueNode) { std::stringstream ss(valueNode->GetValue()); std::string item; while(std::getline(ss, item, ',')) deleted_items.insert(item); }
            } else { final_properties[spec_prop.name] = spec_prop; }
        }
    }
    std::vector<Property> result;
    for (auto const& [name, prop] : final_properties) {
        if (deleted_items.count(name)) continue;
        if (prop.value == nullptr) {
            if (templateDef->IsCustom()) throw std::runtime_error("Valueless property '" + name + "' from custom template '" + templateName + "' was not provided a value.");
        } else { result.push_back(prop); }
    }
    return result;
}
EvaluatedValue CHTLGenerator::EvaluateExpression(const ExpressionNodePtr& expr, const ElementNode* context) {
    if (!expr) return {ValueType::String, ""};
    switch (expr->GetType()) {
        case ExpressionNodeType::StringLiteral: return {ValueType::String, static_cast<const StringLiteralNode*>(expr.get())->GetValue()};
        case ExpressionNodeType::NumberLiteral: return {ValueType::Number, static_cast<const NumberLiteralNode*>(expr.get())->GetValue(), static_cast<const NumberLiteralNode*>(expr.get())->GetUnit()};
        case ExpressionNodeType::BinaryOp: return VisitBinaryOp(static_cast<const BinaryOpNode*>(expr.get()), context);
        case ExpressionNodeType::Conditional: return VisitConditionalExpr(static_cast<const ConditionalExprNode*>(expr.get()), context);
        case ExpressionNodeType::TemplateUsage: throw std::runtime_error("Template usages should be expanded before expression evaluation.");
        case ExpressionNodeType::VariableUsage: return VisitVariableUsage(static_cast<const VariableUsageNode*>(expr.get()), context);
        case ExpressionNodeType::PropertyReference: return VisitPropertyReference(static_cast<const PropertyReferenceNode*>(expr.get()));
        case ExpressionNodeType::SelfPropertyReference: return VisitSelfPropertyReference(static_cast<const SelfPropertyReferenceNode*>(expr.get()), context);
        default: throw std::runtime_error("Unsupported expression type in generator.");
    }
}
EvaluatedValue CHTLGenerator::VisitVariableUsage(const VariableUsageNode* node, const ElementNode* context) {
    if (node->IsSpecialized()) {
        auto const& [varName, varValue] = *node->GetSpecializations().begin();
        return EvaluateExpression(varValue, context);
    }

    std::string full_name = node->GetFrom();
    if (!full_name.empty()) {
        full_name += ".";
    }
    full_name += node->GetGroupName();

    const auto* templateDef = m_context.GetTemplate(full_name);
    if (!templateDef) throw std::runtime_error("Undefined variable group used: " + full_name);

    if (templateDef->GetTemplateType() != TemplateType::Var) throw std::runtime_error("Template is not a variable group: " + full_name);
    const auto* styleNode = static_cast<const StyleNode*>(templateDef->GetContent().front().get());
    for (const auto& prop : styleNode->GetProperties()) {
        if (prop.name == node->GetVariableName()) return EvaluateExpression(prop.value, context);
    }
    throw std::runtime_error("Undefined variable '" + node->GetVariableName() + "' in group '" + node->GetGroupName() + "'");
}

const ElementNode* CHTLGenerator::FindElement(const NodePtr& root, const std::string& selector) {
    if (!root) return nullptr;

    std::function<const ElementNode*(const NodePtr&, const std::string&)> search =
        [&](const NodePtr& currentNode, const std::string& currentSelector) -> const ElementNode* {
        if (!currentNode || currentNode->GetType() != NodeType::Element) return nullptr;

        const auto* element = static_cast<const ElementNode*>(currentNode.get());
        // This is a simplification. A real implementation would parse the selector
        // and match class, id, etc. For now, we only match tag name.
        if (element->GetTagName() == currentSelector) {
            return element;
        }

        for (const auto& child : element->GetChildren()) {
            const ElementNode* found = search(child, currentSelector);
            if (found) return found;
        }

        return nullptr;
    };

    return search(root, selector);
}

EvaluatedValue CHTLGenerator::VisitPropertyReference(const PropertyReferenceNode* node) {
    const ElementNode* targetElement = FindElement(m_astRoot, node->GetSelector());
    if (!targetElement) throw std::runtime_error("Could not find element with selector: " + node->GetSelector());
    for (const auto& prop : static_cast<const ElementNode*>(targetElement)->GetProperties()) {
        if (prop.name == node->GetPropertyName()) {
            return EvaluateExpression(prop.value, targetElement);
        }
    }
    throw std::runtime_error("Could not find property '" + node->GetPropertyName() + "' on element '" + node->GetSelector() + "'");
}
EvaluatedValue CHTLGenerator::VisitSelfPropertyReference(const SelfPropertyReferenceNode* node, const ElementNode* context) {
    if (!context) throw std::runtime_error("Cannot reference self-property outside of an element context.");
    for (const auto& prop : context->GetProperties()) {
        if (prop.name == node->GetPropertyName()) {
            return EvaluateExpression(prop.value, context);
        }
    }
    throw std::runtime_error("Could not find self-property '" + node->GetPropertyName() + "'");
}
EvaluatedValue CHTLGenerator::VisitBinaryOp(const BinaryOpNode* node, const ElementNode* context) {
    EvaluatedValue left = EvaluateExpression(node->GetLeft(), context); EvaluatedValue right = EvaluateExpression(node->GetRight(), context);
    TokenType op = node->GetOperator();
    if (op == TokenType::LogicalAnd || op == TokenType::LogicalOr) {
        bool l_bool = (left.type == ValueType::Bool) ? std::get<bool>(left.value) : (left.type == ValueType::Number && std::get<double>(left.value) != 0);
        bool r_bool = (right.type == ValueType::Bool) ? std::get<bool>(right.value) : (right.type == ValueType::Number && std::get<double>(right.value) != 0);
        return {ValueType::Bool, (op == TokenType::LogicalAnd ? (l_bool && r_bool) : (l_bool || r_bool))};
    }
    if (op == TokenType::Plus && (left.type == ValueType::String || right.type == ValueType::String)) return {ValueType::String, ValueToString(left) + ValueToString(right)};
    if (left.type != ValueType::Number || right.type != ValueType::Number) throw std::runtime_error("Arithmetic and comparison operators require numeric operands.");
    double l_val = std::get<double>(left.value); double r_val = std::get<double>(right.value);
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
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) throw std::runtime_error("Cannot perform arithmetic on numbers with incompatible units.");
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit; double result_val = 0;
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
    if (condition_bool) return EvaluateExpression(node->GetTrueBranch(), context);
    else { if (node->GetFalseBranch()) return EvaluateExpression(node->GetFalseBranch(), context); return {ValueType::String, ""}; }
}

void CHTLGenerator::VisitOrigin(const OriginNode* node) {
    switch (node->GetOriginType()) {
        case OriginType::Html:
            m_output_body << node->GetContent();
            break;
        case OriginType::Style:
            m_output_css << node->GetContent();
            break;
        case OriginType::JavaScript:
            m_output_js << node->GetContent();
            break;
    }
}

} // namespace CHTL
