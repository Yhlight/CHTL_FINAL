#include "ExpressionEvaluator.h"
#include "Expr.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>
#include <vector>

namespace CHTL {

static bool isTruthy(const EvaluatedValue& val) {
    if (!val.unit.empty() && val.unit != "0") return true;
    return val.value != 0;
}

ExpressionEvaluator::ExpressionEvaluator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, BaseNode* doc_root)
    : templates(templates), doc_root(doc_root) {}

EvaluatedValue ExpressionEvaluator::evaluate(Expr* expr, ElementNode* context) {
    this->current_context = context;
    if (expr) {
        expr->accept(*this);
    }
    return result;
}

void ExpressionEvaluator::visit(LiteralExpr& expr) {
    if (expr.unit.empty() || expr.value != 0) {
         result = {expr.value, expr.unit};
    } else {
        result = {0, expr.unit};
    }
}

void ExpressionEvaluator::visit(BinaryExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        throw std::runtime_error("Mismatched units in expression.");
    }
    std::string result_unit = !left.unit.empty() ? left.unit : right.unit;
    double result_value = 0.0;
    switch (expr.op.type) {
        case TokenType::PLUS: result_value = left.value + right.value; break;
        case TokenType::MINUS: result_value = left.value - right.value; break;
        case TokenType::STAR: result_value = left.value * right.value; break;
        case TokenType::SLASH: result_value = right.value == 0 ? 0 : left.value / right.value; break;
        case TokenType::PERCENT: result_value = fmod(left.value, right.value); break;
        case TokenType::STAR_STAR: result_value = pow(left.value, right.value); break;
        default: throw std::runtime_error("Unknown binary operator.");
    }
    result = {result_value, result_unit};
}

void ExpressionEvaluator::visit(VarExpr& expr) {
    // This is a simplified implementation
    if (templates.count(expr.group) && templates.at(expr.group).count(expr.name)) {
        const auto& var_def = templates.at(expr.group).at(expr.name);
        if (var_def.variables.count(expr.name)) {
            result = evaluate(var_def.variables.at(expr.name).get(), this->current_context);
            return;
        }
    }
    throw std::runtime_error("Variable not found: " + expr.group + "." + expr.name);
}

void ExpressionEvaluator::visit(ReferenceExpr& expr) {
    std::string full_ref_name = expr.selector.lexeme + "." + expr.property.lexeme;
    if (resolution_stack.count(full_ref_name)) {
        throw std::runtime_error("Circular property reference detected.");
    }
    ElementNode* target_element = findElement(doc_root, expr.selector.lexeme);
    if (!target_element) {
        throw std::runtime_error("Reference selector not found: " + expr.selector.lexeme);
    }

    for (const auto& child : target_element->children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;

            // Apply templates
            for (const auto& app : styleNode->template_applications) {
                const TemplateDefinitionNode* def = nullptr;
                for (const auto& ns_pair : this->templates) {
                    if (ns_pair.second.count(app.template_name)) {
                        def = &ns_pair.second.at(app.template_name);
                        break;
                    }
                }
                if (def && def->type == TemplateType::STYLE) {
                    for (const auto& prop : def->style_properties) {
                        final_props[prop.key] = prop.clone();
                    }
                    for (const auto& key_to_delete : app.deleted_properties) {
                        final_props.erase(key_to_delete);
                    }
                    for (const auto& prop : app.new_or_overridden_properties) {
                        final_props[prop.key] = prop.clone();
                    }
                }
            }

            // Apply direct properties
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }

            if (final_props.count(expr.property.lexeme)) {
                resolution_stack.insert(full_ref_name);
                result = evaluate(final_props.at(expr.property.lexeme).value_expr.get(), target_element);
                resolution_stack.erase(full_ref_name);
                return;
            }
        }
    }
    throw std::runtime_error("Reference property not found: " + expr.property.lexeme);
}

void ExpressionEvaluator::visit(ComparisonExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    EvaluatedValue right = evaluate(expr.right.get(), this->current_context);
    bool comparison_result = false;
    switch (expr.op.type) {
        case TokenType::GREATER:       comparison_result = left.value > right.value; break;
        case TokenType::GREATER_EQUAL: comparison_result = left.value >= right.value; break;
        case TokenType::LESS:          comparison_result = left.value < right.value; break;
        case TokenType::LESS_EQUAL:    comparison_result = left.value <= right.value; break;
        case TokenType::EQUAL_EQUAL:   comparison_result = left.value == right.value && left.unit == right.unit; break;
        case TokenType::BANG_EQUAL:    comparison_result = left.value != right.value || left.unit != right.unit; break;
        default: break;
    }
    result = {comparison_result ? 1.0 : 0.0, ""};
}

void ExpressionEvaluator::visit(LogicalExpr& expr) {
    EvaluatedValue left = evaluate(expr.left.get(), this->current_context);
    if (expr.op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) { result = left; return; }
    } else {
        if (!isTruthy(left)) { result = left; return; }
    }
    result = evaluate(expr.right.get(), this->current_context);
}

void ExpressionEvaluator::visit(ConditionalExpr& expr) {
    EvaluatedValue condition = evaluate(expr.condition.get(), this->current_context);
    if (isTruthy(condition)) {
        result = evaluate(expr.then_branch.get(), this->current_context);
    } else {
        result = evaluate(expr.else_branch.get(), this->current_context);
    }
}

#include <sstream>
#include <vector>

// Helper function to split string by delimiter
static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static bool elementMatches(ElementNode* element, const std::string& selector) {
    if (!element) return false;

    std::string sel = selector;
    int index = -1;
    size_t bracket_pos = sel.find('[');
    if (bracket_pos != std::string::npos) {
        size_t end_bracket_pos = sel.find(']', bracket_pos);
        if (end_bracket_pos != std::string::npos) {
            index = std::stoi(sel.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
            sel = sel.substr(0, bracket_pos);
        }
    }

    char selector_type = sel.empty() ? ' ' : sel[0];
    if (selector_type == '#') {
        std::string id = sel.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "id" && attr.value == id) return true;
        }
    } else if (selector_type == '.') {
        std::string className = sel.substr(1);
        for (const auto& attr : element->attributes) {
            if (attr.key == "class") {
                std::vector<std::string> classes = split(attr.value, ' ');
                for (const auto& c : classes) {
                    if (c == className) return true;
                }
            }
        }
    } else {
        if (element->tagName == sel) return true;
    }
    return false;
}

static void findElementsRecursive(BaseNode* context, const std::string& selector, std::vector<ElementNode*>& found_elements) {
    if (!context) return;
    ElementNode* element = dynamic_cast<ElementNode*>(context);
    if (!element) return;

    if (elementMatches(element, selector)) {
        found_elements.push_back(element);
    }

    for (const auto& child : element->children) {
        findElementsRecursive(child.get(), selector, found_elements);
    }
}


ElementNode* ExpressionEvaluator::findElement(BaseNode* context, const std::string& selector) {
    std::vector<std::string> parts = split(selector, ' ');
    std::vector<ElementNode*> current_elements;

    BaseNode* search_context = context;

    for (size_t i = 0; i < parts.size(); ++i) {
        std::vector<ElementNode*> next_elements;
        std::string part = parts[i];

        int index = -1;
        size_t bracket_pos = part.find('[');
        if (bracket_pos != std::string::npos) {
            size_t end_bracket_pos = part.find(']', bracket_pos);
            if (end_bracket_pos != std::string::npos) {
                index = std::stoi(part.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
                part = part.substr(0, bracket_pos);
            }
        }

        if (i == 0) {
            findElementsRecursive(search_context, part, next_elements);
        } else {
            for (ElementNode* el : current_elements) {
                for (const auto& child : el->children) {
                    findElementsRecursive(child.get(), part, next_elements);
                }
            }
        }

        current_elements = next_elements;
        if (current_elements.empty()) return nullptr;

        if (index != -1) {
            if (index < current_elements.size()) {
                return current_elements[index];
            } else {
                return nullptr;
            }
        }
    }

    return current_elements.empty() ? nullptr : current_elements[0];
}

} // namespace CHTL
