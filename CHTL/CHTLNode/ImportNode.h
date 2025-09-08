#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

enum class ImportCategory {
    NONE,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    CONFIG,
    CHTL // For importing a whole file
};

enum class ImportType {
    NONE,
    ELEMENT,
    STYLE,
    VAR,
    HTML,
    JAVASCRIPT,
    // For wildcard imports like '[Import] [Custom] from ...'
    ALL
};

class ImportNode : public BaseNode {
public:
    ImportCategory category = ImportCategory::NONE;
    ImportType type = ImportType::NONE;
    std::string path;
    std::string alias;
    // For specific imports like '[Import] [Custom] @Element MyElement from ...'
    std::vector<std::string> specific_imports;

    void accept(AstVisitor& visitor) override {
        // Imports are handled by the parser, so the generator's visitor doesn't need to do anything.
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ImportNode>();
        node->category = this->category;
        node->type = this->type;
        node->path = this->path;
        node->alias = this->alias;
        node->specific_imports = this->specific_imports;
        return node;
    }
};
