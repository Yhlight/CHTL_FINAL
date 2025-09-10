#ifndef CHTL_TEMPLATEUSAGENODE_H
#define CHTL_TEMPLATEUSAGENODE_H

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For the TemplateType enum
#include <string>
#include <sstream>

class TemplateUsageNode : public BaseNode {
public:
    TemplateType type;
    std::string name;

    // Constructor
    TemplateUsageNode(const Token& token, TemplateType type, std::string name)
        : type(type), name(std::move(name)) {
        this->token = token; // The '@Style', '@Element' etc. token
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
        ss << indentString(indent) << "TemplateUsageNode: @" << typeToString() << " " << name << "\n";
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TemplateUsageNode>(*this);
    }
};

#endif //CHTL_TEMPLATEUSAGENODE_H
