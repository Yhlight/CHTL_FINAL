#ifndef CONSTRAINT_NODE_H
#define CONSTRAINT_NODE_H

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class ConstraintType {
    PRECISE, // span, [Custom] @Element Box
    TYPE     // @Html, [Template] @Var, [Custom]
};

struct Constraint {
    ConstraintType type;
    // For PRECISE
    std::string name;
    bool is_custom_element = false; // To distinguish `span` from `[Custom] @Element Box`
    // For TYPE
    TokenType token_type; // @Html -> KEYWORD_HTML (need to add), [Template] -> KEYWORD_TEMPLATE
};

class ConstraintNode : public BaseNode {
public:
    std::vector<Constraint> constraints;

    void accept(Visitor& visitor) override {
        // visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ConstraintNode>(*this);
    }
};

} // namespace CHTL

#endif // CONSTRAINT_NODE_H
