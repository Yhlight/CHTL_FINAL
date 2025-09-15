#include "StyleNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// StyleNode implementation
StyleNode::StyleNode()
    : CHTLBaseNode(NodeType::STYLE), has_selector_(false), inline_style_(false) {
}

StyleNode::~StyleNode() {
}

void StyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    style_properties_[property] = value;
}

const std::map<std::string, std::string>& StyleNode::getStyleProperties() const {
    return style_properties_;
}

void StyleNode::setSelector(const std::string& selector) {
    selector_ = selector;
    has_selector_ = true;
}

const std::string& StyleNode::getSelector() const {
    return selector_;
}

bool StyleNode::hasSelector() const {
    return has_selector_;
}

void StyleNode::setInlineStyle(bool inline_style) {
    inline_style_ = inline_style;
}

bool StyleNode::isInlineStyle() const {
    return inline_style_;
}

void StyleNode::addClassSelector(const std::string& className) {
    class_selectors_.push_back(className);
}

void StyleNode::addIdSelector(const std::string& id) {
    id_selectors_.push_back(id);
}

void StyleNode::addPseudoSelector(const std::string& pseudo) {
    pseudo_selectors_.push_back(pseudo);
}

const std::vector<std::string>& StyleNode::getClassSelectors() const {
    return class_selectors_;
}

const std::vector<std::string>& StyleNode::getIdSelectors() const {
    return id_selectors_;
}

const std::vector<std::string>& StyleNode::getPseudoSelectors() const {
    return pseudo_selectors_;
}

std::string StyleNode::generateCode() const {
    if (inline_style_) {
        // 内联样式
        std::stringstream ss;
        ss << "style=\"";
        bool first = true;
        for (const auto& prop : style_properties_) {
            if (!first) ss << "; ";
            ss << prop.first << ": " << prop.second;
            first = false;
        }
        ss << "\"";
        return ss.str();
    } else {
        // CSS样式块
        std::stringstream ss;
        
        // 选择器
        if (has_selector_) {
            ss << selector_ << " ";
        } else if (!class_selectors_.empty()) {
            ss << "." << class_selectors_[0] << " ";
        } else if (!id_selectors_.empty()) {
            ss << "#" << id_selectors_[0] << " ";
        }
        
        // 伪选择器
        for (const auto& pseudo : pseudo_selectors_) {
            ss << pseudo << " ";
        }
        
        ss << "{\n";
        
        // 样式属性
        for (const auto& prop : style_properties_) {
            ss << "    " << prop.first << ": " << prop.second << ";\n";
        }
        
        ss << "}";
        return ss.str();
    }
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "StyleNode(";
    if (has_selector_) {
        ss << "selector: " << selector_;
    } else if (!class_selectors_.empty()) {
        ss << "class: " << class_selectors_[0];
    } else if (!id_selectors_.empty()) {
        ss << "id: " << id_selectors_[0];
    } else {
        ss << "inline";
    }
    ss << ")";
    return ss.str();
}

// ScriptNode implementation
ScriptNode::ScriptNode()
    : CHTLBaseNode(NodeType::SCRIPT), local_script_(false) {
}

ScriptNode::~ScriptNode() {
}

void ScriptNode::setScriptContent(const std::string& content) {
    script_content_ = content;
}

const std::string& ScriptNode::getScriptContent() const {
    return script_content_;
}

void ScriptNode::setLocalScript(bool local) {
    local_script_ = local;
}

bool ScriptNode::isLocalScript() const {
    return local_script_;
}

void ScriptNode::addCHTLJSSyntax(const std::string& syntax) {
    chtljs_syntaxes_.push_back(syntax);
}

const std::vector<std::string>& ScriptNode::getCHTLJSSyntaxes() const {
    return chtljs_syntaxes_;
}

std::string ScriptNode::generateCode() const {
    std::stringstream ss;
    ss << "<script>\n";
    ss << script_content_ << "\n";
    ss << "</script>";
    return ss.str();
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "ScriptNode(";
    if (local_script_) {
        ss << "local";
    } else {
        ss << "global";
    }
    ss << ")";
    return ss.str();
}

// OriginNode implementation
OriginNode::OriginNode(NodeType type, const std::string& name)
    : CHTLBaseNode(type), name_(name), has_name_(!name.empty()) {
}

OriginNode::~OriginNode() {
}

void OriginNode::setName(const std::string& name) {
    name_ = name;
    has_name_ = true;
}

const std::string& OriginNode::getName() const {
    return name_;
}

bool OriginNode::hasName() const {
    return has_name_;
}

void OriginNode::setContent(const std::string& content) {
    content_ = content;
}

const std::string& OriginNode::getContent() const {
    return content_;
}

std::string OriginNode::generateCode() const {
    return content_;
}

std::string OriginNode::toString() const {
    std::stringstream ss;
    ss << "OriginNode(";
    if (has_name_) {
        ss << "name: " << name_;
    } else {
        ss << "unnamed";
    }
    ss << ")";
    return ss.str();
}

// ImportNode implementation
ImportNode::ImportNode(NodeType type)
    : CHTLBaseNode(type), has_alias_(false), has_namespace_(false) {
}

ImportNode::~ImportNode() {
}

void ImportNode::setPath(const std::string& path) {
    path_ = path;
}

const std::string& ImportNode::getPath() const {
    return path_;
}

void ImportNode::setAlias(const std::string& alias) {
    alias_ = alias;
    has_alias_ = true;
}

const std::string& ImportNode::getAlias() const {
    return alias_;
}

bool ImportNode::hasAlias() const {
    return has_alias_;
}

void ImportNode::setNamespace(const std::string& namespace_name) {
    namespace_name_ = namespace_name;
    has_namespace_ = true;
}

const std::string& ImportNode::getNamespace() const {
    return namespace_name_;
}

bool ImportNode::hasNamespace() const {
    return has_namespace_;
}

std::string ImportNode::generateCode() const {
    std::stringstream ss;
    ss << "[Import] ";
    
    switch (getType()) {
        case NodeType::IMPORT_HTML:
            ss << "@Html";
            break;
        case NodeType::IMPORT_STYLE:
            ss << "@Style";
            break;
        case NodeType::IMPORT_JAVASCRIPT:
            ss << "@JavaScript";
            break;
        case NodeType::IMPORT_CHTL:
            ss << "@Chtl";
            break;
        case NodeType::IMPORT_CJMOD:
            ss << "@CJmod";
            break;
        default:
            ss << "@Unknown";
            break;
    }
    
    ss << " from \"" << path_ << "\"";
    
    if (has_alias_) {
        ss << " as " << alias_;
    }
    
    if (has_namespace_) {
        ss << " from " << namespace_name_;
    }
    
    return ss.str();
}

std::string ImportNode::toString() const {
    std::stringstream ss;
    ss << "ImportNode(" << path_;
    if (has_alias_) {
        ss << ", alias: " << alias_;
    }
    if (has_namespace_) {
        ss << ", namespace: " << namespace_name_;
    }
    ss << ")";
    return ss.str();
}

// NamespaceNode implementation
NamespaceNode::NamespaceNode(const std::string& name)
    : CHTLBaseNode(NodeType::NAMESPACE), name_(name) {
}

NamespaceNode::~NamespaceNode() {
}

void NamespaceNode::setName(const std::string& name) {
    name_ = name;
}

const std::string& NamespaceNode::getName() const {
    return name_;
}

std::string NamespaceNode::generateCode() const {
    std::stringstream ss;
    ss << "[Namespace] " << name_ << "\n";
    ss << "{\n";
    
    for (const auto& child : getChildren()) {
        ss << "    " << child->generateCode() << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string NamespaceNode::toString() const {
    std::stringstream ss;
    ss << "NamespaceNode(" << name_ << ")";
    return ss.str();
}

// ConfigurationNode implementation
ConfigurationNode::ConfigurationNode(const std::string& name)
    : CHTLBaseNode(NodeType::CONFIGURATION), name_(name), has_name_(!name.empty()) {
}

ConfigurationNode::~ConfigurationNode() {
}

void ConfigurationNode::setName(const std::string& name) {
    name_ = name;
    has_name_ = true;
}

const std::string& ConfigurationNode::getName() const {
    return name_;
}

bool ConfigurationNode::hasName() const {
    return has_name_;
}

void ConfigurationNode::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

const std::map<std::string, std::string>& ConfigurationNode::getConfigurations() const {
    return configurations_;
}

std::string ConfigurationNode::generateCode() const {
    std::stringstream ss;
    ss << "[Configuration]";
    if (has_name_) {
        ss << " @Config " << name_;
    }
    ss << "\n{\n";
    
    for (const auto& config : configurations_) {
        ss << "    " << config.first << " = " << config.second << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string ConfigurationNode::toString() const {
    std::stringstream ss;
    ss << "ConfigurationNode(";
    if (has_name_) {
        ss << "name: " << name_;
    } else {
        ss << "unnamed";
    }
    ss << ")";
    return ss.str();
}

} // namespace CHTL