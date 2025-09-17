#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

enum class ImportType {
    Chtl,
    Cjmod,
    Html,
    Style,
    JavaScript,
    Config,
    // For specific imports like [Import] [Custom] @Element ...
    CustomElement,
    CustomStyle,
    CustomVar,
    TemplateElement,
    TemplateStyle,
    TemplateVar,
    Origin,
    Wildcard,
    Unknown
};

class ImportNode : public Node {
public:
    ImportNode(ImportType type, const std::string& path)
        : import_type_(type), path_(path) {}

    NodeType getType() const override { return NodeType::Import; }

    ImportType import_type_;
    std::string path_;
    std::string alias_; // for `as alias`
    std::vector<std::string> specific_imports_; // for `[Import] @Element MyElement ...`

    // For clone
    std::unique_ptr<Node> clone() const override {
        auto node = std::make_unique<ImportNode>(import_type_, path_);
        node->alias_ = alias_;
        node->specific_imports_ = specific_imports_;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_IMPORT_NODE_H
