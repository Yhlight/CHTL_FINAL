#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    explicit ScriptNode(const std::string& content);
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::string content;
    std::string placeholder_key;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H
