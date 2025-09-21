#ifndef ORIGIN_USAGE_NODE_H
#define ORIGIN_USAGE_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginUsageNode : public BaseNode {
public:
    explicit OriginUsageNode(const std::string& name) : name(name) {}

    std::string name;

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;
};

} // namespace CHTL

#endif // ORIGIN_USAGE_NODE_H
