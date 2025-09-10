#ifndef CHTL_CSSPROPERTYNODE_H
#define CHTL_CSSPROPERTYNODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <memory>
#include <sstream>

class CSSPropertyNode : public BaseNode {
public:
    std::string propertyName;
    std::unique_ptr<ExpressionNode> value;

    // Constructor
    CSSPropertyNode(const Token& token, std::string propertyName, std::unique_ptr<ExpressionNode> value)
        : propertyName(std::move(propertyName)), value(std::move(value)) {
        this->token = token;
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "CSSPropertyNode: " << propertyName << "\n";
        ss << value->debugString(indent + 1);
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedValue = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(value->clone().release()));
        return std::make_unique<CSSPropertyNode>(token, propertyName, std::move(clonedValue));
    }
};

#endif //CHTL_CSSPROPERTYNODE_H
