#include "CHTL/CHTLNode.h"
#include "CHTL/CHTLVisitor.h"
#include <algorithm>

namespace CHTL {

// CHTLNode 实现
CHTLNode::CHTLNode(NodeType type, const Position& pos)
    : type_(type), position_(pos) {
}

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void CHTLNode::removeChild(std::shared_ptr<CHTLNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void CHTLNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

void CHTLNode::addAttribute(const Attribute& attr) {
    // 检查是否已存在同名属性
    auto it = std::find_if(attributes_.begin(), attributes_.end(),
        [&attr](const Attribute& a) { return a.name == attr.name; });
    
    if (it != attributes_.end()) {
        *it = attr; // 替换现有属性
    } else {
        attributes_.push_back(attr); // 添加新属性
    }
}

void CHTLNode::setAttribute(const String& name, const String& value, bool quoted) {
    addAttribute(Attribute(name, value, quoted));
}

std::optional<String> CHTLNode::getAttribute(const String& name) const {
    auto it = std::find_if(attributes_.begin(), attributes_.end(),
        [&name](const Attribute& attr) { return attr.name == name; });
    
    if (it != attributes_.end()) {
        return it->value;
    }
    return std::nullopt;
}

void CHTLNode::removeAttribute(const String& name) {
    attributes_.erase(
        std::remove_if(attributes_.begin(), attributes_.end(),
            [&name](const Attribute& attr) { return attr.name == name; }),
        attributes_.end()
    );
}

std::shared_ptr<CHTLNode> CHTLNode::clone() const {
    auto cloned = std::make_shared<CHTLNode>(type_, position_);
    cloned->text_ = text_;
    cloned->tagName_ = tagName_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TextNode 实现
TextNode::TextNode(const String& text, const Position& pos)
    : CHTLNode(NodeType::Text, pos) {
    setText(text);
}

void TextNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(text_, position_);
    return cloned;
}

// ElementNode 实现
ElementNode::ElementNode(const String& tagName, const Position& pos)
    : CHTLNode(NodeType::Element, pos) {
    setTagName(tagName);
}

void ElementNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::findElements(const String& tagName) const {
    std::vector<std::shared_ptr<ElementNode>> result;
    
    for (const auto& child : children_) {
        if (child->getType() == NodeType::Element) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getTagName() == tagName) {
                result.push_back(element);
            }
            
            // 递归查找子元素
            auto childElements = element->findElements(tagName);
            result.insert(result.end(), childElements.begin(), childElements.end());
        }
    }
    
    return result;
}

std::shared_ptr<ElementNode> ElementNode::findElement(const String& tagName) const {
    for (const auto& child : children_) {
        if (child->getType() == NodeType::Element) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getTagName() == tagName) {
                return element;
            }
            
            // 递归查找
            auto found = element->findElement(tagName);
            if (found) {
                return found;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::findElementsByClass(const String& className) const {
    std::vector<std::shared_ptr<ElementNode>> result;
    
    for (const auto& child : children_) {
        if (child->getType() == NodeType::Element) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            auto classAttr = element->getAttribute("class");
            if (classAttr && classAttr->find(className) != String::npos) {
                result.push_back(element);
            }
            
            // 递归查找
            auto childElements = element->findElementsByClass(className);
            result.insert(result.end(), childElements.begin(), childElements.end());
        }
    }
    
    return result;
}

std::shared_ptr<ElementNode> ElementNode::findElementById(const String& id) const {
    for (const auto& child : children_) {
        if (child->getType() == NodeType::Element) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            auto idAttr = element->getAttribute("id");
            if (idAttr && *idAttr == id) {
                return element;
            }
            
            // 递归查找
            auto found = element->findElementById(id);
            if (found) {
                return found;
            }
        }
    }
    
    return nullptr;
}

// CommentNode 实现
CommentNode::CommentNode(const String& content, const Position& pos)
    : CHTLNode(NodeType::Comment, pos) {
    setText(content);
}

void CommentNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(text_, position_);
    cloned->commentType_ = commentType_;
    return cloned;
}

// StyleNode 实现
StyleNode::StyleNode(const Position& pos)
    : CHTLNode(NodeType::Style, pos) {
}

void StyleNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(position_);
    cloned->styleContent_ = styleContent_;
    cloned->isLocal_ = isLocal_;
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ScriptNode 实现
ScriptNode::ScriptNode(const Position& pos)
    : CHTLNode(NodeType::Script, pos) {
}

void ScriptNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(position_);
    cloned->scriptContent_ = scriptContent_;
    cloned->isLocal_ = isLocal_;
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TemplateNode 实现
TemplateNode::TemplateNode(NodeType type, const String& name, const Position& pos)
    : CHTLNode(type, pos), templateName_(name) {
}

// TemplateStyleNode 实现
TemplateStyleNode::TemplateStyleNode(const String& name, const Position& pos)
    : TemplateNode(NodeType::TemplateStyle, name, pos) {
}

void TemplateStyleNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TemplateStyleNode::clone() const {
    auto cloned = std::make_shared<TemplateStyleNode>(templateName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->inherits_ = inherits_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TemplateElementNode 实现
TemplateElementNode::TemplateElementNode(const String& name, const Position& pos)
    : TemplateNode(NodeType::TemplateElement, name, pos) {
}

void TemplateElementNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TemplateElementNode::clone() const {
    auto cloned = std::make_shared<TemplateElementNode>(templateName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->inherits_ = inherits_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TemplateVarNode 实现
TemplateVarNode::TemplateVarNode(const String& name, const Position& pos)
    : TemplateNode(NodeType::TemplateVar, name, pos) {
}

void TemplateVarNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TemplateVarNode::clone() const {
    auto cloned = std::make_shared<TemplateVarNode>(templateName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->inherits_ = inherits_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// CustomNode 实现
CustomNode::CustomNode(NodeType type, const String& name, const Position& pos)
    : CHTLNode(type, pos), customName_(name) {
}

// CustomStyleNode 实现
CustomStyleNode::CustomStyleNode(const String& name, const Position& pos)
    : CustomNode(NodeType::CustomStyle, name, pos) {
}

void CustomStyleNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CustomStyleNode::clone() const {
    auto cloned = std::make_shared<CustomStyleNode>(customName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->deletes_ = deletes_;
    cloned->inserts_ = inserts_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// CustomElementNode 实现
CustomElementNode::CustomElementNode(const String& name, const Position& pos)
    : CustomNode(NodeType::CustomElement, name, pos) {
}

void CustomElementNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CustomElementNode::clone() const {
    auto cloned = std::make_shared<CustomElementNode>(customName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->deletes_ = deletes_;
    cloned->inserts_ = inserts_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// CustomVarNode 实现
CustomVarNode::CustomVarNode(const String& name, const Position& pos)
    : CustomNode(NodeType::CustomVar, name, pos) {
}

void CustomVarNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CustomVarNode::clone() const {
    auto cloned = std::make_shared<CustomVarNode>(customName_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    cloned->deletes_ = deletes_;
    cloned->inserts_ = inserts_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// OriginNode 实现
OriginNode::OriginNode(const String& type, const String& name, const Position& pos)
    : CHTLNode(NodeType::Origin, pos), originType_(type), originName_(name) {
}

void OriginNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(originType_, originName_, position_);
    cloned->originContent_ = originContent_;
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ImportNode 实现
ImportNode::ImportNode(const String& type, const String& path, const String& alias, const Position& pos)
    : CHTLNode(NodeType::Import, pos), importType_(type), importPath_(path), importAlias_(alias) {
}

void ImportNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(importType_, importPath_, importAlias_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ConfigNode 实现
ConfigNode::ConfigNode(const String& name, const Position& pos)
    : CHTLNode(NodeType::Config, pos), configName_(name) {
}

void ConfigNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ConfigNode::clone() const {
    auto cloned = std::make_shared<ConfigNode>(configName_, position_);
    cloned->configOptions_ = configOptions_;
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// NamespaceNode 实现
NamespaceNode::NamespaceNode(const String& name, const Position& pos)
    : CHTLNode(NodeType::Namespace, pos), namespaceName_(name) {
}

void NamespaceNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespaceName_, position_);
    cloned->parentNamespace_ = parentNamespace_;
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// OperatorNode 实现
OperatorNode::OperatorNode(OperatorType type, const Position& pos)
    : CHTLNode(NodeType::Operator, pos), operatorType_(type) {
}

void OperatorNode::accept(CHTLVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> OperatorNode::clone() const {
    auto cloned = std::make_shared<OperatorNode>(operatorType_, position_);
    cloned->text_ = text_;
    cloned->selfClosing_ = selfClosing_;
    
    // 克隆属性
    for (const auto& attr : attributes_) {
        cloned->addAttribute(attr);
    }
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

} // namespace CHTL