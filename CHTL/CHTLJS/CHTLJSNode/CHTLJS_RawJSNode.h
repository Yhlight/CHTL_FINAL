#ifndef CHTLJS_RAWJS_NODE_H
#define CHTLJS_RAWJS_NODE_H

#include "CHTLJS_BaseNode.h"
#include <string>
#include <memory>

namespace CHTLJS {

class CHTLJS_RawJSNode : public CHTLJS_BaseNode {
public:
    std::string js_code;

    explicit CHTLJS_RawJSNode(std::string code) : js_code(std::move(code)) {}

    void accept(CHTLJS_Visitor& visitor) override;

    std::unique_ptr<CHTLJS_BaseNode> clone() const override {
        return std::make_unique<CHTLJS_RawJSNode>(js_code);
    }
};

} // namespace CHTLJS

#endif // CHTLJS_RAWJS_NODE_H
