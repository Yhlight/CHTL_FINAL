#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

struct ImportInfo {
    // e.g., "[Custom]", "[Template]", "[Origin]"
    std::string import_block_type;
    // e.g., "@Element", "@Style"
    std::string import_specifier;
    // Name of the thing to import, or "*" for wildcard
    std::string name;
    std::string path;
    // Optional alias
    std::string alias;
};

class ImportNode : public BaseNode {
public:
    ImportInfo info;

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

}

#endif //IMPORT_NODE_H
