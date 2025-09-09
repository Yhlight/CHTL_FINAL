#include "AnimateNode.hpp"
#include <sstream>

namespace CHTL {

AnimateNode::AnimateNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::ANIMATE, line, column), 
      duration_(1000), easing_("ease-in-out"), loop_(1), delay_(0) {}

void AnimateNode::setTarget(const std::string& target) {
    target_ = target;
}

void AnimateNode::setDuration(int duration) {
    duration_ = duration;
}

void AnimateNode::setEasing(const std::string& easing) {
    easing_ = easing;
}

void AnimateNode::setBeginState(const std::map<std::string, std::string>& properties) {
    beginState_ = properties;
}

void AnimateNode::addKeyframe(const Keyframe& keyframe) {
    keyframes_.push_back(keyframe);
}

void AnimateNode::setEndState(const std::map<std::string, std::string>& properties) {
    endState_ = properties;
}

void AnimateNode::setLoop(int loop) {
    loop_ = loop;
}

void AnimateNode::setDirection(const std::string& direction) {
    direction_ = direction;
}

void AnimateNode::setDelay(int delay) {
    delay_ = delay;
}

void AnimateNode::setCallback(const std::string& callback) {
    callback_ = callback;
}

std::string AnimateNode::toJavaScript() const {
    std::ostringstream oss;
    
    oss << "// Animation\n";
    oss << "(function() {\n";
    oss << "    const target = document.querySelector('" << target_ << "');\n";
    oss << "    if (!target) return;\n";
    oss << "    \n";
    oss << "    const animation = {\n";
    oss << "        duration: " << duration_ << ",\n";
    oss << "        easing: '" << easing_ << "',\n";
    oss << "        loop: " << loop_ << ",\n";
    oss << "        delay: " << delay_ << ",\n";
    oss << "        direction: '" << direction_ << "',\n";
    
    // 起始状态
    if (!beginState_.empty()) {
        oss << "        begin: {\n";
        oss << generateCSSProperties(beginState_);
        oss << "        },\n";
    }
    
    // 关键帧
    if (!keyframes_.empty()) {
        oss << "        keyframes: [\n";
        for (size_t i = 0; i < keyframes_.size(); ++i) {
            const auto& keyframe = keyframes_[i];
            oss << "            {\n";
            oss << "                at: " << keyframe.at << ",\n";
            oss << "                properties: {\n";
            oss << generateCSSProperties(keyframe.properties);
            oss << "                }\n";
            oss << "            }";
            if (i < keyframes_.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        oss << "        ],\n";
    }
    
    // 结束状态
    if (!endState_.empty()) {
        oss << "        end: {\n";
        oss << generateCSSProperties(endState_);
        oss << "        },\n";
    }
    
    // 回调函数
    if (!callback_.empty()) {
        oss << "        callback: " << callback_ << ",\n";
    }
    
    oss << "    };\n";
    oss << "    \n";
    oss << "    // Start animation using requestAnimationFrame\n";
    oss << "    let startTime = null;\n";
    oss << "    let currentLoop = 0;\n";
    oss << "    \n";
    oss << "    function animate(timestamp) {\n";
    oss << "        if (!startTime) startTime = timestamp;\n";
    oss << "        \n";
    oss << "        const elapsed = timestamp - startTime;\n";
    oss << "        const progress = Math.min(elapsed / animation.duration, 1);\n";
    oss << "        \n";
    oss << "        // Apply easing\n";
    oss << "        let easedProgress = progress;\n";
    oss << "        switch (animation.easing) {\n";
    oss << "            case 'ease-in':\n";
    oss << "                easedProgress = progress * progress;\n";
    oss << "                break;\n";
    oss << "            case 'ease-out':\n";
    oss << "                easedProgress = 1 - Math.pow(1 - progress, 2);\n";
    oss << "                break;\n";
    oss << "            case 'ease-in-out':\n";
    oss << "                easedProgress = progress < 0.5 ? 2 * progress * progress : 1 - Math.pow(-2 * progress + 2, 2) / 2;\n";
    oss << "                break;\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        // Apply animation properties\n";
    oss << "        // This is a simplified implementation\n";
    oss << "        // In a real implementation, you would interpolate between keyframes\n";
    oss << "        \n";
    oss << "        if (progress < 1) {\n";
    oss << "            requestAnimationFrame(animate);\n";
    oss << "        } else {\n";
    oss << "            currentLoop++;\n";
    oss << "            if (animation.loop === -1 || currentLoop < animation.loop) {\n";
    oss << "                startTime = null;\n";
    oss << "                requestAnimationFrame(animate);\n";
    oss << "            } else if (animation.callback) {\n";
    oss << "                animation.callback();\n";
    oss << "            }\n";
    oss << "        }\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    setTimeout(() => {\n";
    oss << "        requestAnimationFrame(animate);\n";
    oss << "    }, animation.delay);\n";
    oss << "})();\n";
    
    return oss.str();
}

std::string AnimateNode::toString() const {
    std::ostringstream oss;
    oss << "AnimateNode(target=\"" << target_ 
        << "\", duration=" << duration_ 
        << ", keyframes=" << keyframes_.size() 
        << ", line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string AnimateNode::generateCSSProperties(const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    for (const auto& [property, value] : properties) {
        oss << "            '" << property << "': '" << value << "',\n";
    }
    return oss.str();
}

} // namespace CHTL