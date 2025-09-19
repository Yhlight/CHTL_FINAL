#ifndef SCRIPT_NODE_H
#define SCRIPT_NODE_H

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
};

} // namespace CHTL

#endif // SCRIPT_NODE_H
