#ifndef ORIGIN_NODE_H
#define ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    OriginNode(std::string type, std::string content, std::string name = "");
    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    std::string type;
    std::string content;
    std::string name; // Optional name for the block
};

} // namespace CHTL

#endif // ORIGIN_NODE_H
