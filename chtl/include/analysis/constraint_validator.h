#pragma once

#include "ast/ast_node.h"
#include <memory>
#include <vector>

namespace chtl {
namespace analysis {

class ConstraintValidator {
public:
    ConstraintValidator();

    // Traverses the AST and validates all constraints.
    // Returns true if validation passes, false otherwise.
    bool validate(ast::ASTNode::NodePtr root);

    const std::vector<std::string>& get_errors() const;

private:
    void visit_node(ast::ASTNode::NodePtr node);

    std::vector<std::string> errors_;
};

} // namespace analysis
} // namespace chtl
