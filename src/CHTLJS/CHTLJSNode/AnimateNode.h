#ifndef CHTL_JS_ANIMATE_NODE_H
#define CHTL_JS_ANIMATE_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

namespace CHTLJS {

struct Keyframe {
    std::optional<double> offset; // 0.0 to 1.0
    std::map<std::string, std::string> properties;
};

class AnimateNode : public CHTLJSNode {
public:
    AnimateNode() : CHTLJSNode(CHTLJSNodeType::Animate) {}

    std::vector<std::unique_ptr<CHTLJSNode>> targets_;
    std::optional<std::string> duration_;
    std::optional<std::string> easing_;
    std::optional<std::string> loop_;
    std::optional<std::string> direction_;
    std::optional<std::string> delay_;
    std::optional<std::string> callback_;
    std::vector<Keyframe> keyframes_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ANIMATE_NODE_H
