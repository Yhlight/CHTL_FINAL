#include "ConfigurationVisitor.h"
#include "CHTLNode/ExprNode.h" // To get to LiteralExprNode

ConfigurationVisitor::ConfigurationVisitor(ConfigurationContext& context) : context(context) {}

void ConfigurationVisitor::run(const NodeList& ast) {
    for (const auto& node : ast) {
        // We only need to visit top-level nodes.
        // A configuration block inside another element is not valid according to the spec.
        if (dynamic_cast<ConfigurationNode*>(node.get())) {
            node->accept(*this);
        }
    }
}

void ConfigurationVisitor::visit(ConfigurationNode* node) {
    for (const auto& setting : node->settings) {
        if (auto* literal = dynamic_cast<LiteralExprNode*>(setting->value.get())) {
            if (setting->name.rfind("KEYWORD_", 0) == 0) {
                // This is a keyword redefinition. The spec implies a replacement, not an addition.
                context.clearKeyword(setting->name);
                context.addKeyword(setting->name, literal->value.value);
            } else {
                context.setString(setting->name, literal->value.value);
            }
        }
    }
}
