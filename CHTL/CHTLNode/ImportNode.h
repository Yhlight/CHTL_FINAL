#ifndef IMPORT_NODE_H
#define IMPORT_NODE_H

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {

enum class ImportType {
    HTML,
    STYLE,
    JAVASCRIPT,
    CHTL,
    CJMOD,
    CONFIG,
    UNKNOWN
};

class ImportNode : public BaseNode {
public:
    ImportType import_type;
    // For precise imports
    bool is_custom = false;
    TokenType precise_type = TokenType::UNKNOWN; // @Element, @Style, etc.
    std::string item_name;

    std::string path;
    std::string alias;

    void accept(Visitor& visitor) override {
        // visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(*this);
    }
};

} // namespace CHTL

#endif // IMPORT_NODE_H
