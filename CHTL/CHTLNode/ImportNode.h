#pragma once

#include "Node.h"
#include <string>
#include <utility>

namespace CHTL {

// For now, we only support full CHTL file imports.
enum class ImportType {
    CHTL_FILE
};

class ImportNode : public Node {
public:
    ImportNode(ImportType type, std::string path)
        : import_type(type), path(std::move(path)) {}

    std::string ToString(int indent = 0) const override {
        return std::string(indent, ' ') + "Import( path=\"" + path + "\" )";
    }

    NodePtr clone() const override {
        return std::make_shared<ImportNode>(import_type, path);
    }

    ImportType import_type;
    std::string path;
};

} // namespace CHTL
