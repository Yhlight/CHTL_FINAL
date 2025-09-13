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
    AnimateNode() = default;

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Animate; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto new_node = std::make_unique<AnimateNode>();
        for (const auto& t : targets_) {
            new_node->targets_.push_back(t->clone());
        }
        new_node->duration_ = this->duration_;
        new_node->easing_ = this->easing_;
        new_node->loop_ = this->loop_;
        new_node->direction_ = this->direction_;
        new_node->delay_ = this->delay_;
        new_node->callback_ = this->callback_;
        new_node->keyframes_ = this->keyframes_;
        return new_node;
    }

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
