#ifndef CHTL_TEMPLATEDEFINITIONNODE_H
#define CHTL_TEMPLATEDEFINITIONNODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateType type;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> body;

    // Constructor
    TemplateDefinitionNode(const Token& token, TemplateType type, std::string name)
        : type(type), name(std::move(name)) {
        this->token = token; // The '[Template]' keyword token
    }

    void addNode(std::unique_ptr<BaseNode> node) {
        body.push_back(std::move(node));
    }

    std::string typeToString() const {
        switch (type) {
            case TemplateType::STYLE: return "Style";
            case TemplateType::ELEMENT: return "Element";
            case TemplateType::VAR: return "Var";
            default: return "Unknown";
        }
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "TemplateDefinitionNode: " << name << " (" << typeToString() << ")\n";
        for (const auto& node : body) {
            ss << node->debugString(indent + 1);
        }
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<TemplateDefinitionNode>(token, type, name);
        for (const auto& node : body) {
            clonedNode->addNode(node->clone());
        }
        return clonedNode;
    }
};

#endif //CHTL_TEMPLATEDEFINITIONNODE_H
