#ifndef CHTLJS_LISTEN_NODE_H
#define CHTLJS_LISTEN_NODE_H

#include "CHTLJS_BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

struct EventHandler {
    std::string event_name;
    std::string handler_code; // For now, just the raw string of the function
};

class CHTLJS_ListenNode : public CHTLJS_BaseNode {
public:
    std::unique_ptr<CHTLJS_BaseNode> target;
    std::vector<EventHandler> handlers;

    explicit CHTLJS_ListenNode(std::unique_ptr<CHTLJS_BaseNode> target)
        : target(std::move(target)) {}

    void accept(CHTLJS_Visitor& visitor) override;

    std::unique_ptr<CHTLJS_BaseNode> clone() const override;
};

} // namespace CHTLJS

#endif // CHTLJS_LISTEN_NODE_H
