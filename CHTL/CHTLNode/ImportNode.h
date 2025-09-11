#ifndef CHTL_IMPORTNODE_H
#define CHTL_IMPORTNODE_H

#include "BaseNode.h"
#include <string>
#include <sstream>

// Using a simple string for type for now, as it's very complex.
// e.g., "[Custom] @Element", "@Chtl", "@Css"
// An enum would be very large.

class ImportNode : public BaseNode {
public:
    std::string importTypeString; // e.g., "[Custom] @Element"
    std::string path;
    std::string itemName; // The specific item to import, if not a wildcard
    std::string alias;    // The alias name, if 'as' is used

    // Constructor
    ImportNode(const Token& token, std::string typeStr, std::string path, std::string item, std::string alias)
        : importTypeString(std::move(typeStr)),
          path(std::move(path)),
          itemName(std::move(item)),
          alias(std::move(alias)) {
        this->token = token; // The '[Import]' keyword token
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "ImportNode: " << importTypeString;
        if (!itemName.empty()) {
            ss << " " << itemName;
        }
        ss << " from \"" << path << "\"";
        if (!alias.empty()) {
            ss << " as " << alias;
        }
        ss << "\n";
        return ss.str();
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(*this);
    }
};

#endif //CHTL_IMPORTNODE_H
