#include "CHTL/CHTLNode/ScriptNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

ScriptNode::ScriptNode(ScriptType type) 
    : BaseNode(NodeType::SCRIPT), m_scriptType(type) {
}

ScriptNode::~ScriptNode() = default;

ScriptType ScriptNode::getScriptType() const {
    return m_scriptType;
}

void ScriptNode::setScriptContent(const std::string& content) {
    m_scriptContent = content;
}

std::string ScriptNode::getScriptContent() const {
    return m_scriptContent;
}

void ScriptNode::addCHTLJSFunction(const std::string& name, const std::string& content) {
    m_chtljsFunctions[name] = content;
}

std::string ScriptNode::getCHTLJSFunction(const std::string& name) const {
    auto it = m_chtljsFunctions.find(name);
    return (it != m_chtljsFunctions.end()) ? it->second : "";
}

bool ScriptNode::hasCHTLJSFunction(const std::string& name) const {
    return m_chtljsFunctions.find(name) != m_chtljsFunctions.end();
}

const std::unordered_map<std::string, std::string>& ScriptNode::getCHTLJSFunctions() const {
    return m_chtljsFunctions;
}

void ScriptNode::addSelector(const std::string& selector) {
    m_selectors.push_back(selector);
}

std::vector<std::string> ScriptNode::getSelectors() const {
    return m_selectors;
}

bool ScriptNode::hasSelector(const std::string& selector) const {
    return std::find(m_selectors.begin(), m_selectors.end(), selector) != m_selectors.end();
}

void ScriptNode::addListener(const std::string& event, const std::string& handler) {
    m_listeners[event] = handler;
}

std::unordered_map<std::string, std::string> ScriptNode::getListeners() const {
    return m_listeners;
}

void ScriptNode::addDelegate(const std::string& parentSelector, const std::string& targetSelector, 
                           const std::string& event, const std::string& handler) {
    m_delegates.push_back({parentSelector, targetSelector, event, handler});
}

std::vector<std::tuple<std::string, std::string, std::string, std::string>> ScriptNode::getDelegates() const {
    return m_delegates;
}

void ScriptNode::addAnimation(const std::string& name, const std::string& animationDef) {
    m_animations[name] = animationDef;
}

std::string ScriptNode::getAnimation(const std::string& name) const {
    auto it = m_animations.find(name);
    return (it != m_animations.end()) ? it->second : "";
}

bool ScriptNode::hasAnimation(const std::string& name) const {
    return m_animations.find(name) != m_animations.end();
}

const std::unordered_map<std::string, std::string>& ScriptNode::getAnimations() const {
    return m_animations;
}

void ScriptNode::addVirtualObject(const std::string& name, const std::string& definition) {
    m_virtualObjects[name] = definition;
}

std::string ScriptNode::getVirtualObject(const std::string& name) const {
    auto it = m_virtualObjects.find(name);
    return (it != m_virtualObjects.end()) ? it->second : "";
}

bool ScriptNode::hasVirtualObject(const std::string& name) const {
    return m_virtualObjects.find(name) != m_virtualObjects.end();
}

const std::unordered_map<std::string, std::string>& ScriptNode::getVirtualObjects() const {
    return m_virtualObjects;
}

void ScriptNode::addRoute(const std::string& url, const std::string& page) {
    m_routes[url] = page;
}

std::string ScriptNode::getRoute(const std::string& url) const {
    auto it = m_routes.find(url);
    return (it != m_routes.end()) ? it->second : "";
}

bool ScriptNode::hasRoute(const std::string& url) const {
    return m_routes.find(url) != m_routes.end();
}

const std::unordered_map<std::string, std::string>& ScriptNode::getRoutes() const {
    return m_routes;
}

void ScriptNode::addFileLoader(const std::string& filePath) {
    m_fileLoaders.push_back(filePath);
}

std::vector<std::string> ScriptNode::getFileLoaders() const {
    return m_fileLoaders;
}

void ScriptNode::addDynamicCondition(const std::string& property, const std::string& condition) {
    m_dynamicConditions.push_back({property, condition});
}

std::vector<std::pair<std::string, std::string>> ScriptNode::getDynamicConditions() const {
    return m_dynamicConditions;
}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visitScript(*this);
}

std::shared_ptr<BaseNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(m_scriptType);
    cloned->m_scriptContent = m_scriptContent;
    cloned->m_chtljsFunctions = m_chtljsFunctions;
    cloned->m_selectors = m_selectors;
    cloned->m_listeners = m_listeners;
    cloned->m_delegates = m_delegates;
    cloned->m_animations = m_animations;
    cloned->m_virtualObjects = m_virtualObjects;
    cloned->m_routes = m_routes;
    cloned->m_fileLoaders = m_fileLoaders;
    cloned->m_dynamicConditions = m_dynamicConditions;
    cloned->m_content = m_content;
    cloned->m_name = m_name;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    cloned->m_attributes = m_attributes;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ScriptNode::validate() const {
    return BaseNode::validate();
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "ScriptNode{type=" << static_cast<int>(m_scriptType) 
       << ", content='" << m_scriptContent << "', chtljsFunctions=" << m_chtljsFunctions.size() 
       << ", selectors=" << m_selectors.size() << ", listeners=" << m_listeners.size() 
       << ", delegates=" << m_delegates.size() << ", animations=" << m_animations.size() 
       << ", virtualObjects=" << m_virtualObjects.size() << ", routes=" << m_routes.size() 
       << ", fileLoaders=" << m_fileLoaders.size() << ", dynamicConditions=" << m_dynamicConditions.size() << "}";
    return ss.str();
}

} // namespace CHTL