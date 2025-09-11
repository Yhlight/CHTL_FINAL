#include "ExprEvaluator.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include <stdexcept>
#include <iostream>

// --- EvaluatedValue ---
std::string EvaluatedValue::toString() const {
    if (type == Type::String) {
        return string_val;
    }
    return std::to_string(double_val) + unit;
}

// --- ExprEvaluator ---
ExprEvaluator::ExprEvaluator(Node& ast_root) : ast_root(ast_root) {}

EvaluatedValue ExprEvaluator::evaluate(ExprNode& expr) {
    expr.accept(*this);
    return last_value;
}

void ExprEvaluator::visit(LiteralNode* node) {
    const std::string& val = node->value.value;
    size_t first_digit = val.find_first_of("-.0123456789");
    if (first_digit == 0) {
        size_t first_nondigit = val.find_first_not_of(".0123456789", first_digit == 0 ? 1 : 0);
        try {
            last_value.double_val = std::stod(val.substr(0, first_nondigit));
            last_value.unit = first_nondigit != std::string::npos ? val.substr(first_nondigit) : "";
            last_value.type = EvaluatedValue::Type::Double;
            return;
        } catch (const std::invalid_argument&) {}
    }
    last_value.string_val = val;
    if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
        last_value.string_val = val.substr(1, val.length() - 2);
    }
    last_value.type = EvaluatedValue::Type::String;
}

void ExprEvaluator::visit(UnaryExprNode* node) {
    node->right->accept(*this);
    if (node->op.type == TokenType::Minus) {
        if (last_value.type == EvaluatedValue::Type::Double) {
            last_value.double_val = -last_value.double_val;
        } else {
            last_value.string_val = "-" + last_value.string_val;
        }
    }
}

void ExprEvaluator::visit(BinaryExprNode* node) {
    node->left->accept(*this);
    EvaluatedValue left_val = last_value;
    node->right->accept(*this);
    EvaluatedValue right_val = last_value;

    if (left_val.type == EvaluatedValue::Type::Double && right_val.type == EvaluatedValue::Type::Double) {
        if (!left_val.unit.empty() && !right_val.unit.empty() && left_val.unit != right_val.unit) {
            throw std::runtime_error("Mismatched units in binary expression: " + left_val.unit + " and " + right_val.unit);
        }
        double result = 0.0;
        if (node->op.type == TokenType::Plus) result = left_val.double_val + right_val.double_val;
        else if (node->op.type == TokenType::Minus) result = left_val.double_val - right_val.double_val;
        else if (node->op.type == TokenType::Asterisk) result = left_val.double_val * right_val.double_val;
        else if (node->op.type == TokenType::Slash) result = left_val.double_val / right_val.double_val;
        else throw std::runtime_error("Unsupported binary operator for numeric types: " + node->op.value);
        last_value = { EvaluatedValue::Type::Double, "", result, left_val.unit.empty() ? right_val.unit : left_val.unit };
    } else {
        if (node->op.type == TokenType::Plus) {
            last_value = { EvaluatedValue::Type::String, left_val.toString() + right_val.toString() };
        } else {
            throw std::runtime_error("Unsupported binary operator for string types: " + node->op.value);
        }
    }
}

void ExprEvaluator::visit(TernaryExprNode* node) {
    node->condition->accept(*this);
    EvaluatedValue cond_val = last_value;
    bool is_true = (cond_val.type == EvaluatedValue::Type::Double) ? (cond_val.double_val != 0) : (!cond_val.string_val.empty() && cond_val.string_val != "false");
    if (is_true) node->trueExpr->accept(*this);
    else node->falseExpr->accept(*this);
}

class NodeFinder : public Visitor {
public:
    NodeFinder(const std::string& selector) : selector(selector) {}
    ElementNode* find(Node& root) {
        root.accept(*this);
        return foundNode;
    }
    void visit(ElementNode* node) override {
        if (foundNode) return;
        if (selector.front() == '#') {
            for (const auto& child : node->children) {
                if (auto* prop = dynamic_cast<PropertyNode*>(child.get())) {
                    if (prop->name == "id" && prop->value && dynamic_cast<LiteralExprNode*>(prop->value.get()) && dynamic_cast<LiteralExprNode*>(prop->value.get())->value.value == selector.substr(1)) {
                        foundNode = node; return;
                    }
                }
            }
        } else if (node->tagName == selector) {
            foundNode = node; return;
        }
        for (const auto& child : node->children) child->accept(*this);
    }
    void visit(NamespaceNode* node) override { for (const auto& child : node->children) child->accept(*this); }
    void visit(TextNode* n) override{} void visit(CommentNode* n) override{} void visit(PropertyNode* n) override{} void visit(StyleNode* n) override{} void visit(ScriptNode* n) override{} void visit(StyleTemplateDefinitionNode* n) override{} void visit(ElementTemplateDefinitionNode* n) override{} void visit(VarTemplateDefinitionNode* n) override{} void visit(StyleUsageNode* n) override{} void visit(ElementUsageNode* n) override{} void visit(CustomStyleDefinitionNode* n) override{} void visit(CustomElementDefinitionNode* n) override{} void visit(CustomVarDefinitionNode* n) override{} void visit(DeleteNode* n) override{} void visit(InsertNode* n) override{} void visit(ImportNode* n) override{} void visit(ConfigurationNode* n) override{} void visit(UseNode* n) override{} void visit(ConstraintNode* n) override{}
private:
    std::string selector;
    ElementNode* foundNode = nullptr;
};

PropertyNode* ExprEvaluator::findProperty(const std::string& selector, const std::string& propertyName) {
    NodeFinder finder(selector);
    ElementNode* targetElement = finder.find(ast_root);
    if (targetElement) {
        for (const auto& child : targetElement->children) {
            if (auto* style = dynamic_cast<StyleNode*>(child.get())) {
                for(const auto& styleChild : style->children) {
                    if (auto* prop = dynamic_cast<PropertyNode*>(styleChild.get()) && prop->name == propertyName) return prop;
                }
            }
            if (auto* prop = dynamic_cast<PropertyNode*>(child.get()) && prop->name == propertyName) return prop;
        }
    }
    return nullptr;
}

void ExprEvaluator::visit(PropertyRefNode* node) {
    PropertyNode* referenced_prop = findProperty(node->selector.value, node->propertyName.value);
    if (!referenced_prop) throw std::runtime_error("Could not find property '" + node->propertyName.value + "' on selector '" + node->selector.value + "'.");
    if (evaluation_cache.count(referenced_prop)) {
        if (evaluation_cache.at(referenced_prop).type == EvaluatedValue::Type::String && evaluation_cache.at(referenced_prop).string_val == "cyclic_dependency_error") {
            throw std::runtime_error("Cyclic property dependency detected involving selector '" + node->selector.value + "'.");
        }
        last_value = evaluation_cache.at(referenced_prop);
        return;
    }
    evaluation_cache[referenced_prop] = { EvaluatedValue::Type::String, "cyclic_dependency_error" };
    EvaluatedValue result = evaluate(*referenced_prop->value);
    evaluation_cache[referenced_prop] = result;
    last_value = result;
}
