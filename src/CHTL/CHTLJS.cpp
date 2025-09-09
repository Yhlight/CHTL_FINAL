#include "CHTL/CHTLJS.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSFileLoader 实现
CHTLJSFileLoader::CHTLJSFileLoader(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSFileLoader::loadFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        m_context->addError("Cannot open CHTL JS file: " + path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLJSFileLoader::fileExists(const std::string& path) const {
    std::ifstream file(path);
    return file.good();
}

std::vector<std::string> CHTLJSFileLoader::parseDependencies(const std::string& content) const {
    std::vector<std::string> dependencies;
    
    // 解析 AMD define 依赖
    std::regex defineRegex(R"(define\s*\(\s*\[([^\]]*)\])");
    std::smatch match;
    
    if (std::regex_search(content, match, defineRegex)) {
        std::string depsStr = match[1].str();
        std::stringstream ss(depsStr);
        std::string dep;
        
        while (std::getline(ss, dep, ',')) {
            // 去除引号和空格
            dep.erase(std::remove(dep.begin(), dep.end(), '"'), dep.end());
            dep.erase(std::remove(dep.begin(), dep.end(), '\''), dep.end());
            dep.erase(0, dep.find_first_not_of(" \t"));
            dep.erase(dep.find_last_not_of(" \t") + 1);
            
            if (!dep.empty()) {
                dependencies.push_back(dep);
            }
        }
    }
    
    return dependencies;
}

std::string CHTLJSFileLoader::resolveModulePath(const std::string& moduleName, const std::string& basePath) const {
    // 简单的模块路径解析
    if (moduleName.starts_with("./") || moduleName.starts_with("../")) {
        return basePath + "/" + moduleName;
    } else if (moduleName.starts_with("/")) {
        return moduleName;
    } else {
        return basePath + "/" + moduleName + ".js";
    }
}

void CHTLJSFileLoader::addDependency(const std::string& moduleName, const std::string& path) {
    m_dependencies[moduleName] = path;
}

std::vector<std::string> CHTLJSFileLoader::getDependencies(const std::string& moduleName) const {
    std::vector<std::string> deps;
    auto it = m_dependencies.find(moduleName);
    if (it != m_dependencies.end()) {
        deps.push_back(it->second);
    }
    return deps;
}

void CHTLJSFileLoader::cacheModule(const std::string& moduleName, const std::string& content) {
    m_moduleCache[moduleName] = content;
}

std::string CHTLJSFileLoader::getCachedModule(const std::string& moduleName) const {
    auto it = m_moduleCache.find(moduleName);
    return (it != m_moduleCache.end()) ? it->second : "";
}

bool CHTLJSFileLoader::isModuleCached(const std::string& moduleName) const {
    return m_moduleCache.find(moduleName) != m_moduleCache.end();
}

std::string CHTLJSFileLoader::extractModuleName(const std::string& path) const {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string filename = path.substr(lastSlash + 1);
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos) {
            return filename.substr(0, lastDot);
        }
        return filename;
    }
    return path;
}

std::vector<std::string> CHTLJSFileLoader::parseAMDDefine(const std::string& content) const {
    return parseDependencies(content);
}

// CHTLJSSelector 实现
CHTLJSSelector::CHTLJSSelector(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSSelector::parseSelector(const std::string& selector) const {
    if (selector.empty()) {
        return "";
    }
    
    // 处理 {{CSS选择器}} 语法
    if (selector.starts_with("{{") && selector.ends_with("}}")) {
        std::string innerSelector = selector.substr(2, selector.length() - 4);
        return applyAutomationRules(innerSelector);
    }
    
    return applyAutomationRules(selector);
}

std::vector<std::string> CHTLJSSelector::parseMultipleSelectors(const std::string& selectors) const {
    std::vector<std::string> result;
    std::stringstream ss(selectors);
    std::string selector;
    
    while (std::getline(ss, selector, ',')) {
        selector.erase(0, selector.find_first_not_of(" \t"));
        selector.erase(selector.find_last_not_of(" \t") + 1);
        
        if (!selector.empty()) {
            result.push_back(parseSelector(selector));
        }
    }
    
    return result;
}

bool CHTLJSSelector::isValidSelector(const std::string& selector) const {
    if (selector.empty()) {
        return false;
    }
    
    // 基本的 CSS 选择器验证
    std::regex validSelectorRegex(R"(^[a-zA-Z0-9\s\.\#\[\]:,>+~*\(\)]+$)");
    return std::regex_match(selector, validSelectorRegex);
}

bool CHTLJSSelector::isComplexSelector(const std::string& selector) const {
    return selector.find(' ') != std::string::npos ||
           selector.find('>') != std::string::npos ||
           selector.find('+') != std::string::npos ||
           selector.find('~') != std::string::npos;
}

std::string CHTLJSSelector::generateSelectorCode(const std::string& selector) const {
    if (!isValidSelector(selector)) {
        m_context->addError("Invalid CSS selector: " + selector);
        return "";
    }
    
    std::string parsedSelector = parseSelector(selector);
    return "document.querySelector('" + escapeSelector(parsedSelector) + "')";
}

std::string CHTLJSSelector::generateMultipleSelectorCode(const std::vector<std::string>& selectors) const {
    if (selectors.empty()) {
        return "[]";
    }
    
    std::stringstream ss;
    ss << "[";
    
    for (size_t i = 0; i < selectors.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << generateSelectorCode(selectors[i]);
    }
    
    ss << "]";
    return ss.str();
}

std::string CHTLJSSelector::applyAutomationRules(const std::string& selector) const {
    std::string result = selector;
    
    // 应用自动化规则
    if (isElementSelector(result)) {
        result = addClassPrefix(result);
    }
    
    return result;
}

std::string CHTLJSSelector::addClassPrefix(const std::string& selector) const {
    // 添加类前缀的自动化规则
    if (selector.find('.') == std::string::npos && selector.find('#') == std::string::npos) {
        return "." + selector;
    }
    return selector;
}

std::string CHTLJSSelector::addIdPrefix(const std::string& selector) const {
    // 添加 ID 前缀的自动化规则
    if (selector.find('.') == std::string::npos && selector.find('#') == std::string::npos) {
        return "#" + selector;
    }
    return selector;
}

bool CHTLJSSelector::isElementSelector(const std::string& selector) const {
    return !selector.empty() && 
           selector[0] != '.' && 
           selector[0] != '#' && 
           selector[0] != '[' &&
           selector[0] != ':';
}

bool CHTLJSSelector::isClassSelector(const std::string& selector) const {
    return selector.starts_with(".");
}

bool CHTLJSSelector::isIdSelector(const std::string& selector) const {
    return selector.starts_with("#");
}

bool CHTLJSSelector::isAttributeSelector(const std::string& selector) const {
    return selector.starts_with("[");
}

bool CHTLJSSelector::isPseudoSelector(const std::string& selector) const {
    return selector.starts_with(":");
}

std::string CHTLJSSelector::escapeSelector(const std::string& selector) const {
    std::string result = selector;
    
    // 转义特殊字符
    std::replace(result.begin(), result.end(), '\'', '\\');
    std::replace(result.begin(), result.end(), '"', '\\');
    
    return result;
}

// CHTLJSEventListener 实现
CHTLJSEventListener::CHTLJSEventListener(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSEventListener::parseEventListener(const std::string& eventType, const std::string& handler) const {
    if (!isValidEventType(eventType)) {
        m_context->addError("Invalid event type: " + eventType);
        return "";
    }
    
    return generateEventListenerCode(eventType, handler);
}

std::vector<std::pair<std::string, std::string>> CHTLJSEventListener::parseMultipleListeners(const std::string& listeners) const {
    std::vector<std::pair<std::string, std::string>> result;
    
    // 解析多个事件监听器
    std::regex listenerRegex(R"(\s*(\w+)\s*:\s*([^,]+)(?:,|$))");
    std::sregex_iterator iter(listeners.begin(), listeners.end(), listenerRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string eventType = match[1].str();
        std::string handler = match[2].str();
        
        // 去除前后空格
        handler.erase(0, handler.find_first_not_of(" \t"));
        handler.erase(handler.find_last_not_of(" \t") + 1);
        
        result.push_back({eventType, handler});
    }
    
    return result;
}

std::string CHTLJSEventListener::generateEventListenerCode(const std::string& eventType, const std::string& handler) const {
    std::string sanitizedHandler = sanitizeEventHandler(handler);
    
    return "element.addEventListener('" + eventType + "', function(event) {\n" +
           "  " + sanitizedHandler + "\n" +
           "});";
}

std::string CHTLJSEventListener::generateMultipleListenerCode(const std::vector<std::pair<std::string, std::string>>& listeners) const {
    if (listeners.empty()) {
        return "";
    }
    
    std::stringstream ss;
    
    for (const auto& [eventType, handler] : listeners) {
        ss << generateEventListenerCode(eventType, handler) << "\n";
    }
    
    return ss.str();
}

std::string CHTLJSEventListener::generateEventDelegationCode(const std::string& target, const std::string& eventType, const std::string& handler) const {
    return "document.addEventListener('" + eventType + "', function(event) {\n" +
           "  if (event.target.matches('" + target + "')) {\n" +
           "    " + sanitizeEventHandler(handler) + "\n" +
           "  }\n" +
           "});";
}

std::string CHTLJSEventListener::parseEventDelegation(const std::string& delegation) const {
    // 解析事件委托语法
    std::regex delegationRegex(R"(target\s*:\s*([^,]+),\s*(\w+)\s*:\s*([^,]+))");
    std::smatch match;
    
    if (std::regex_search(delegation, match, delegationRegex)) {
        std::string target = match[1].str();
        std::string eventType = match[2].str();
        std::string handler = match[3].str();
        
        return generateEventDelegationCode(target, eventType, handler);
    }
    
    return "";
}

bool CHTLJSEventListener::isValidEventType(const std::string& eventType) const {
    static const std::set<std::string> validEvents = {
        "click", "mouseenter", "mouseleave", "mousedown", "mouseup",
        "keydown", "keyup", "keypress", "focus", "blur",
        "change", "input", "submit", "load", "unload",
        "resize", "scroll", "touchstart", "touchend", "touchmove"
    };
    
    return validEvents.find(eventType) != validEvents.end();
}

std::string CHTLJSEventListener::sanitizeEventHandler(const std::string& handler) const {
    std::string result = handler;
    
    // 基本的代码清理
    result.erase(0, result.find_first_not_of(" \t"));
    result.erase(result.find_last_not_of(" \t") + 1);
    
    return result;
}

std::string CHTLJSEventListener::generateEventDelegationHandler(const std::string& target, const std::string& eventType, const std::string& handler) const {
    return generateEventDelegationCode(target, eventType, handler);
}

} // namespace CHTL