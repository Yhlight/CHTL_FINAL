#ifndef CHTL_CALLEXPRESSIONNODE_H
#define CHTL_CALLEXPRESSIONNODE_H

#include "ExpressionNode.h"
#include <vector>
#include <memory>
#include <sstream>

class CallExpressionNode : public ExpressionNode {
public:
    // The function being called, e.g., the identifier 'ThemeColor'
    std::unique_ptr<ExpressionNode> function;
    // The list of arguments, e.g., the identifier 'tableColor'
    std::vector<std::unique_ptr<ExpressionNode>> arguments;

    // Constructor
    CallExpressionNode(const Token& token, std::unique_ptr<ExpressionNode> function)
        : function(std::move(function)) {
        this->token = token; // The '(' token
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "CallExpressionNode:\n";
        ss << function->debugString(indent + 1);
        for (const auto& arg : arguments) {
            ss << arg->debugString(indent + 1);
        }
        return ss.str();
    }
};

#endif //CHTL_CALLEXPRESSIONNODE_H
