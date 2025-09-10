#ifndef CHTL_ATTRIBUTENODE_H
#define CHTL_ATTRIBUTENODE_H

#include "BaseNode.h"
#include <string>

class AttributeNode : public BaseNode {
public:
    std::string name;
    std::string value;

    // Constructor
    AttributeNode(const Token& token, std::string name, std::string value)
        : name(std::move(name)), value(std::move(value)) {
        this->token = token;
    }

    std::string debugString(int indent = 0) const override {
        return indentString(indent) + "AttributeNode: " + name + "=\"" + value + "\"\n";
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<AttributeNode>(*this);
    }
};

#endif //CHTL_ATTRIBUTENODE_H
