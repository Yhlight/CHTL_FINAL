#include "RootNode.h"
#include <sstream>

namespace CHTL {

RootNode::RootNode() : BaseNode(NodeType::ROOT, "root") {
}

std::string RootNode::toHTML() const {
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        oss << child->toHTML();
    }
    return oss.str();
}

std::string RootNode::toCSS() const {
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string css = child->toCSS();
        if (!css.empty()) {
            oss << css << "\n";
        }
    }
    return oss.str();
}

std::string RootNode::toJS() const {
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string js = child->toJS();
        if (!js.empty()) {
            oss << js << "\n";
        }
    }
    return oss.str();
}

std::shared_ptr<BaseNode> RootNode::clone() const {
    auto cloned = std::make_shared<RootNode>();
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

} // namespace CHTL