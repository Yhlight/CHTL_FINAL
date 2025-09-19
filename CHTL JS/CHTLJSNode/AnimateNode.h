#ifndef ANIMATE_NODE_H
#define ANIMATE_NODE_H

#include "CHTLJSBaseNode.h"
#include "EnhancedSelectorNode.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace CHTL_JS {

struct Keyframe {
    double at;
    std::map<std::string, std::string> properties;
};

class AnimateNode : public CHTLJSBaseNode {
public:
    AnimateNode() {
        type = CHTLJSNodeType::Animate;
    }

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::vector<ParsedSelector> targets;
    std::optional<int> duration;
    std::optional<std::string> easing;
    std::map<std::string, std::string> begin_state;
    std::vector<Keyframe> when_keyframes;
    std::map<std::string, std::string> end_state;
    std::optional<int> loop;
    std::optional<std::string> direction;
    std::optional<int> delay;
    std::optional<std::string> callback;
};

} // namespace CHTL_JS

#endif // ANIMATE_NODE_H
