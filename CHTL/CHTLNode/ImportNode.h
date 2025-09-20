#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include "BaseNode.h"
#include <string>
#include <utility>

namespace CHTL {

enum class ImportType {
    CHTL,
    HTML,
    STYLE,
    JAVASCRIPT,
    // More types for specific imports later
};

class ImportNode : public BaseNode {
public:
    ImportType type;
    std::string path;
    std::string alias; // Optional 'as' name

    ImportNode(ImportType type, std::string path, std::string alias = "")
        : type(type), path(std::move(path)), alias(std::move(alias)) {}

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // IMPORT_NODE_H
