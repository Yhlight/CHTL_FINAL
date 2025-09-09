#include "ScriptNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

ScriptNode::ScriptNode(ScriptType type, const std::string& content) 
    : BaseNode(NodeType::SCRIPT, "script"), scriptType(type), scriptContent(content),
      autoAddClass(true), autoAddId(true) {
}

void ScriptNode::setParameter(const std::string& key, const std::string& value) {
    parameters[key] = value;
}

std::string ScriptNode::getParameter(const std::string& key) const {
    auto it = parameters.find(key);
    if (it != parameters.end()) {
        return it->second;
    }
    return "";
}

bool ScriptNode::hasParameter(const std::string& key) const {
    return parameters.find(key) != parameters.end();
}

void ScriptNode::removeParameter(const std::string& key) {
    parameters.erase(key);
}

void ScriptNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void ScriptNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void ScriptNode::addFileLoaderPath(const std::string& path) {
    fileLoaderPaths.push_back(path);
}

void ScriptNode::addEventListener(const std::string& event, const std::string& handler) {
    eventListeners[event] = handler;
}

void ScriptNode::addDelegate(const std::string& target, const std::string& config) {
    delegates[target] = config;
}

void ScriptNode::addAnimation(const std::string& name, const std::string& config) {
    animations[name] = config;
}

void ScriptNode::addVirtualObject(const std::string& name, const std::string& config) {
    virtualObjects[name] = config;
}

void ScriptNode::addRoute(const std::string& path, const std::string& config) {
    routes[path] = config;
}

void ScriptNode::addSelector(const std::string& selector) {
    selectors.push_back(selector);
}

std::shared_ptr<BaseNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(scriptType, scriptContent);
    cloned->parameters = parameters;
    cloned->fileLoaderPaths = fileLoaderPaths;
    cloned->eventListeners = eventListeners;
    cloned->delegates = delegates;
    cloned->animations = animations;
    cloned->virtualObjects = virtualObjects;
    cloned->routes = routes;
    cloned->selectors = selectors;
    cloned->contextSelector = contextSelector;
    cloned->autoAddClass = autoAddClass;
    cloned->autoAddId = autoAddId;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ScriptNode::toString() const {
    std::ostringstream oss;
    oss << "ScriptNode(" << static_cast<int>(scriptType) << ", \"" << scriptContent << "\")";
    return oss.str();
}

std::string ScriptNode::toHTML() const {
    // 脚本节点不直接生成HTML
    return "";
}

std::string ScriptNode::toCSS() const {
    // 脚本节点不直接生成CSS
    return "";
}

std::string ScriptNode::toJS() const {
    std::string result;
    
    switch (scriptType) {
        case ScriptType::INLINE:
        case ScriptType::GLOBAL:
            result = processCHTLJSSyntax(scriptContent);
            break;
        case ScriptType::CHTL_JS:
            result = processCHTLJSSyntax(scriptContent);
            break;
        case ScriptType::FILELOADER:
            result = generateFileLoaderJS(fileLoaderPaths);
            break;
        case ScriptType::LISTEN:
            result = generateEventListenerJS(eventListeners);
            break;
        case ScriptType::DELEGATE:
            result = generateDelegateJS(delegates);
            break;
        case ScriptType::ANIMATE:
            result = generateAnimationJS(animations);
            break;
        case ScriptType::ROUTER:
            result = generateRouterJS(routes);
            break;
        default:
            result = scriptContent;
            break;
    }
    
    return result;
}

bool ScriptNode::isValid() const {
    return BaseNode::isValid();
}

std::string ScriptNode::debugString() const {
    std::ostringstream oss;
    oss << "ScriptNode: " << scriptContent << " (Type: " << static_cast<int>(scriptType) 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

std::string ScriptNode::processCHTLJSSyntax(const std::string& content) {
    std::string result = content;
    
    // 处理增强选择器 {{selector}}
    result = processSelectors(result);
    
    // 处理箭头操作符 ->
    result = processArrows(result);
    
    return result;
}

std::string ScriptNode::processSelectors(const std::string& content) {
    std::string result = content;
    
    // 匹配 {{selector}} 模式
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    while (std::regex_search(result, match, selectorRegex)) {
        std::string selector = match[1].str();
        std::string replacement;
        
        // 根据选择器类型生成不同的代码
        if (selector[0] == '.') {
            // 类选择器
            replacement = "document.querySelector('" + selector + "')";
        } else if (selector[0] == '#') {
            // ID选择器
            replacement = "document.getElementById('" + selector.substr(1) + "')";
        } else {
            // 标签选择器
            replacement = "document.querySelector('" + selector + "')";
        }
        
        result = std::regex_replace(result, selectorRegex, replacement);
    }
    
    return result;
}

std::string ScriptNode::processArrows(const std::string& content) {
    std::string result = content;
    
    // 将 -> 替换为 .
    std::regex arrowRegex(R"(\->)");
    result = std::regex_replace(result, arrowRegex, ".");
    
    return result;
}

std::string ScriptNode::generateFileLoaderJS(const std::vector<std::string>& paths) {
    std::ostringstream oss;
    
    oss << "// CHTL JS File Loader\n";
    oss << "(function() {\n";
    oss << "    const loadScript = (src) => {\n";
    oss << "        return new Promise((resolve, reject) => {\n";
    oss << "            const script = document.createElement('script');\n";
    oss << "            script.src = src;\n";
    oss << "            script.onload = resolve;\n";
    oss << "            script.onerror = reject;\n";
    oss << "            document.head.appendChild(script);\n";
    oss << "        });\n";
    oss << "    };\n\n";
    
    oss << "    const loadCSS = (href) => {\n";
    oss << "        return new Promise((resolve, reject) => {\n";
    oss << "            const link = document.createElement('link');\n";
    oss << "            link.rel = 'stylesheet';\n";
    oss << "            link.href = href;\n";
    oss << "            link.onload = resolve;\n";
    oss << "            link.onerror = reject;\n";
    oss << "            document.head.appendChild(link);\n";
    oss << "        });\n";
    oss << "    };\n\n";
    
    oss << "    const loadFiles = async () => {\n";
    for (const auto& path : paths) {
        if (path.ends_with(".css")) {
            oss << "        await loadCSS('" << path << "');\n";
        } else {
            oss << "        await loadScript('" << path << "');\n";
        }
    }
    oss << "    };\n\n";
    
    oss << "    loadFiles().catch(console.error);\n";
    oss << "})();\n";
    
    return oss.str();
}

std::string ScriptNode::generateEventListenerJS(const std::map<std::string, std::string>& listeners) {
    std::ostringstream oss;
    
    for (const auto& listener : listeners) {
        oss << "element.addEventListener('" << listener.first << "', " << listener.second << ");\n";
    }
    
    return oss.str();
}

std::string ScriptNode::generateDelegateJS(const std::map<std::string, std::string>& delegates) {
    std::ostringstream oss;
    
    oss << "// Event Delegation\n";
    for (const auto& delegate : delegates) {
        oss << "parentElement.addEventListener('click', (e) => {\n";
        oss << "    if (e.target.matches('" << delegate.first << "')) {\n";
        oss << "        " << delegate.second << "\n";
        oss << "    }\n";
        oss << "});\n";
    }
    
    return oss.str();
}

std::string ScriptNode::generateAnimationJS(const std::map<std::string, std::string>& animations) {
    std::ostringstream oss;
    
    oss << "// CHTL JS Animation\n";
    for (const auto& anim : animations) {
        oss << "const " << anim.first << " = {\n";
        oss << "    target: " << anim.second << ",\n";
        oss << "    duration: 1000,\n";
        oss << "    easing: 'ease-in-out',\n";
        oss << "    start: () => {},\n";
        oss << "    end: () => {}\n";
        oss << "};\n";
    }
    
    return oss.str();
}

std::string ScriptNode::generateRouterJS(const std::map<std::string, std::string>& routes) {
    std::ostringstream oss;
    
    oss << "// CHTL JS Router\n";
    oss << "const router = {\n";
    oss << "    routes: {\n";
    for (const auto& route : routes) {
        oss << "        '" << route.first << "': " << route.second << ",\n";
    }
    oss << "    },\n";
    oss << "    navigate: (path) => {\n";
    oss << "        if (this.routes[path]) {\n";
    oss << "            this.routes[path]();\n";
    oss << "        }\n";
    oss << "    }\n";
    oss << "};\n";
    
    return oss.str();
}

} // namespace CHTL