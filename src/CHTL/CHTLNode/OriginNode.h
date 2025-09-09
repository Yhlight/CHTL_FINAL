#ifndef ORIGIN_NODE_H
#define ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& content);

    void accept(Visitor& visitor) override;
    std::unique_ptr<BaseNode> clone() const override;

    const std::string& getType() const;
    const std::string& getContent() const;

private:
    std::string type;
    std::string content;
};

#endif // ORIGIN_NODE_H
