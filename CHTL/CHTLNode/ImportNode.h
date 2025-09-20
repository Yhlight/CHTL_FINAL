#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <vector>

namespace CHTL {

// The category of the import, e.g., [Template], [Custom], [Origin]
enum class ImportCategory {
    None,
    Template,
    Custom,
    Origin,
    Configuration,
};

// The type of the import, e.g., @Chtl, @Style, @Element
enum class ImportType {
    Chtl,
    Html,
    Style,
    JavaScript,
    CJmod,
    Config,
    Element,
    Var,
};


// Represents an [Import] statement
class ImportNode : public BaseNode {
public:
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    ImportCategory category = ImportCategory::None;
    ImportType type;
    std::string name; // The specific name of the item to import
    std::string path;
    std::string alias;
};

} // namespace CHTL

#endif // IMPORT_NODE_H
