#include "analysis/constraint_validator.h"
#include "ast/ast_node.h"
#include <sstream>

namespace chtl {
namespace analysis {

ConstraintValidator::ConstraintValidator() {}

bool ConstraintValidator::validate(ast::ASTNode::NodePtr root) {
    errors_.clear();
    if (!root) {
        return true;
    }
    visit_node(root);
    return errors_.empty();
}

const std::vector<std::string>& ConstraintValidator::get_errors() const {
    return errors_;
}

void ConstraintValidator::visit_node(ast::ASTNode::NodePtr node) {
    if (!node) {
        return;
    }

    if (node->type == ast::NodeType::ELEMENT) {
        auto element_node = std::static_pointer_cast<ast::ElementNode>(node);
        if (!element_node->constraints.empty()) {
            for (const auto& child : element_node->children) {
                for (const auto& constraint : element_node->constraints) {
                    // Simple name check for now. A full implementation would need
                    // to parse the constraint string (e.g., "[Custom] @Element Box").
                    if (child->name == constraint) {
                        std::ostringstream error_msg;
                        error_msg << "Constraint violation at line " << element_node->line
                                  << ": Element '" << element_node->name
                                  << "' cannot contain child '" << child->name << "'.";
                        errors_.push_back(error_msg.str());
                    }
                }
            }
        }
    }

    // Recurse on children
    for (const auto& child : node->children) {
        visit_node(child);
    }
}

} // namespace analysis
} // namespace chtl
