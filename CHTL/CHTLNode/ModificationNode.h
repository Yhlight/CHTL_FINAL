#ifndef MODIFICATION_NODE_H
#define MODIFICATION_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class InsertPosition {
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM
};

class InsertNode : public BaseNode {
public:
    InsertNode(InsertPosition position, const std::string& selector);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    InsertPosition getPosition() const;
    const std::string& getSelector() const;

private:
    InsertPosition m_position;
    std::string m_selector;
};

class DeleteNode : public BaseNode {
public:
    DeleteNode(const std::string& selector);

    NodeType getType() const override;
    std::unique_ptr<BaseNode> clone() const override; // Added

    const std::string& getSelector() const;

private:
    std::string m_selector;
};

} // namespace CHTL

#endif // MODIFICATION_NODE_H
