#include "CHTL/CHTLJSCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// CHTLJSFileLoader 实现
CHTLJSFileLoader::CHTLJSFileLoader() = default;

CHTLJSFileLoader::~CHTLJSFileLoader() = default;

void CHTLJSFileLoader::addFile(const std::string& filePath) {
    if (std::find(files_.begin(), files_.end(), filePath) == files_.end()) {
        files_.push_back(filePath);
    }
}

void CHTLJSFileLoader::addFiles(const std::vector<std::string>& filePaths) {
    for (const auto& filePath : filePaths) {
        addFile(filePath);
    }
}

void CHTLJSFileLoader::removeFile(const std::string& filePath) {
    files_.erase(std::remove(files_.begin(), files_.end(), filePath), files_.end());
}

void CHTLJSFileLoader::clearFiles() {
    files_.clear();
}

std::vector<std::string> CHTLJSFileLoader::getFiles() const {
    return files_;
}

bool CHTLJSFileLoader::hasFile(const std::string& filePath) const {
    return std::find(files_.begin(), files_.end(), filePath) != files_.end();
}

std::string CHTLJSFileLoader::processFiles() const {
    std::ostringstream oss;
    
    for (const auto& filePath : files_) {
        if (fileExists(filePath)) {
            std::string content = readFile(filePath);
            oss << "// File: " << filePath << std::endl;
            oss << content << std::endl;
            oss << std::endl;
        }
    }
    
    return oss.str();
}

std::vector<std::string> CHTLJSFileLoader::getFileContents() const {
    std::vector<std::string> contents;
    
    for (const auto& filePath : files_) {
        if (fileExists(filePath)) {
            contents.push_back(readFile(filePath));
        }
    }
    
    return contents;
}

bool CHTLJSFileLoader::validateFiles() const {
    for (const auto& filePath : files_) {
        if (!fileExists(filePath)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CHTLJSFileLoader::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& filePath : files_) {
        if (!fileExists(filePath)) {
            errors.push_back("File not found: " + filePath);
        }
    }
    
    return errors;
}

std::string CHTLJSFileLoader::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool CHTLJSFileLoader::fileExists(const std::string& filePath) const {
    std::ifstream file(filePath);
    return file.good();
}

std::string CHTLJSFileLoader::getFileExtension(const std::string& filePath) const {
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos) {
        return filePath.substr(pos + 1);
    }
    return "";
}

// CHTLJSEnhancedSelector 实现
CHTLJSEnhancedSelector::CHTLJSEnhancedSelector() = default;

CHTLJSEnhancedSelector::~CHTLJSEnhancedSelector() = default;

CHTLJSEnhancedSelector::SelectorType CHTLJSEnhancedSelector::parseSelector(const std::string& selector) const {
    if (selector.empty()) {
        return SelectorType::TAG;
    }
    
    if (selector[0] == '.') {
        return SelectorType::CLASS;
    } else if (selector[0] == '#') {
        return SelectorType::ID;
    } else if (selector.find(' ') != std::string::npos) {
        return SelectorType::DESCENDANT;
    } else if (selector.find('[') != std::string::npos) {
        return SelectorType::INDEXED;
    } else {
        return SelectorType::TAG;
    }
}

std::string CHTLJSEnhancedSelector::generateJavaScript(const std::string& selector) const {
    SelectorType type = parseSelector(selector);
    
    switch (type) {
        case SelectorType::TAG:
            return generateTagSelector(extractTagName(selector));
        case SelectorType::CLASS:
            return generateClassSelector(extractClassName(selector));
        case SelectorType::ID:
            return generateIdSelector(extractIdName(selector));
        case SelectorType::DESCENDANT:
            return generateDescendantSelector(selector);
        case SelectorType::INDEXED:
            return generateIndexedSelector(extractTagName(selector), extractIndex(selector));
        default:
            return "document.querySelector('" + selector + "')";
    }
}

bool CHTLJSEnhancedSelector::validateSelector(const std::string& selector) const {
    if (selector.empty()) {
        return false;
    }
    
    // 基本验证规则
    if (selector.find("{{") != 0 || selector.find("}}") == std::string::npos) {
        return false;
    }
    
    return true;
}

std::vector<std::string> CHTLJSEnhancedSelector::getValidationErrors(const std::string& selector) const {
    std::vector<std::string> errors;
    
    if (selector.empty()) {
        errors.push_back("Selector cannot be empty");
        return errors;
    }
    
    if (selector.find("{{") != 0) {
        errors.push_back("Selector must start with {{");
    }
    
    if (selector.find("}}") == std::string::npos) {
        errors.push_back("Selector must end with }}");
    }
    
    return errors;
}

std::string CHTLJSEnhancedSelector::extractTagName(const std::string& selector) const {
    std::string clean = selector;
    if (clean.find("{{") == 0) {
        clean = clean.substr(2);
    }
    if (clean.find("}}") != std::string::npos) {
        clean = clean.substr(0, clean.find("}}"));
    }
    if (clean.find("[") != std::string::npos) {
        clean = clean.substr(0, clean.find("["));
    }
    return clean;
}

std::string CHTLJSEnhancedSelector::extractClassName(const std::string& selector) const {
    std::string clean = selector;
    if (clean.find("{{") == 0) {
        clean = clean.substr(2);
    }
    if (clean.find("}}") != std::string::npos) {
        clean = clean.substr(0, clean.find("}}"));
    }
    if (clean[0] == '.') {
        clean = clean.substr(1);
    }
    return clean;
}

std::string CHTLJSEnhancedSelector::extractIdName(const std::string& selector) const {
    std::string clean = selector;
    if (clean.find("{{") == 0) {
        clean = clean.substr(2);
    }
    if (clean.find("}}") != std::string::npos) {
        clean = clean.substr(0, clean.find("}}"));
    }
    if (clean[0] == '#') {
        clean = clean.substr(1);
    }
    return clean;
}

std::string CHTLJSEnhancedSelector::extractDescendantSelector(const std::string& selector) const {
    std::string clean = selector;
    if (clean.find("{{") == 0) {
        clean = clean.substr(2);
    }
    if (clean.find("}}") != std::string::npos) {
        clean = clean.substr(0, clean.find("}}"));
    }
    return clean;
}

int CHTLJSEnhancedSelector::extractIndex(const std::string& selector) const {
    size_t start = selector.find("[");
    size_t end = selector.find("]");
    
    if (start != std::string::npos && end != std::string::npos) {
        std::string indexStr = selector.substr(start + 1, end - start - 1);
        try {
            return std::stoi(indexStr);
        } catch (const std::exception&) {
            return 0;
        }
    }
    
    return 0;
}

std::string CHTLJSEnhancedSelector::generateTagSelector(const std::string& tagName) const {
    return "document.querySelector('" + tagName + "')";
}

std::string CHTLJSEnhancedSelector::generateClassSelector(const std::string& className) const {
    return "document.querySelector('." + className + "')";
}

std::string CHTLJSEnhancedSelector::generateIdSelector(const std::string& idName) const {
    return "document.querySelector('#" + idName + "')";
}

std::string CHTLJSEnhancedSelector::generateDescendantSelector(const std::string& descendantSelector) const {
    return "document.querySelector('" + descendantSelector + "')";
}

std::string CHTLJSEnhancedSelector::generateIndexedSelector(const std::string& baseSelector, int index) const {
    return "document.querySelectorAll('" + baseSelector + "')[" + std::to_string(index) + "]";
}

// CHTLJSAnimationSystem 实现
CHTLJSAnimationSystem::CHTLJSAnimationSystem() = default;

CHTLJSAnimationSystem::~CHTLJSAnimationSystem() = default;

std::string CHTLJSAnimationSystem::generateAnimation(const AnimationConfig& config) const {
    std::ostringstream oss;
    
    oss << "animate({" << std::endl;
    oss << "    target: " << generateTargetSelector(config.target) << "," << std::endl;
    oss << "    duration: " << config.duration << "," << std::endl;
    oss << "    easing: '" << config.easing << "'," << std::endl;
    
    if (!config.begin.empty()) {
        oss << "    begin: {" << std::endl;
        for (const auto& pair : config.begin) {
            oss << "        " << pair.first << ": '" << pair.second << "'," << std::endl;
        }
        oss << "    }," << std::endl;
    }
    
    if (!config.when.empty()) {
        oss << "    when: [" << std::endl;
        for (const auto& when : config.when) {
            oss << "        {" << std::endl;
            for (const auto& pair : when) {
                oss << "            " << pair.first << ": '" << pair.second << "'," << std::endl;
            }
            oss << "        }," << std::endl;
        }
        oss << "    ]," << std::endl;
    }
    
    if (!config.end.empty()) {
        oss << "    end: {" << std::endl;
        for (const auto& pair : config.end) {
            oss << "        " << pair.first << ": '" << pair.second << "'," << std::endl;
        }
        oss << "    }," << std::endl;
    }
    
    oss << "    loop: " << config.loop << "," << std::endl;
    oss << "    direction: '" << config.direction << "'," << std::endl;
    oss << "    delay: " << config.delay << "," << std::endl;
    oss << "    callback: " << generateCallback(config.callback) << std::endl;
    oss << "});" << std::endl;
    
    return oss.str();
}

std::string CHTLJSAnimationSystem::generateAnimationFunction(const AnimationConfig& config) const {
    std::ostringstream oss;
    
    oss << "function animate(config) {" << std::endl;
    oss << "    const element = config.target;" << std::endl;
    oss << "    const duration = config.duration;" << std::endl;
    oss << "    const easing = config.easing;" << std::endl;
    oss << "    const loop = config.loop;" << std::endl;
    oss << "    const direction = config.direction;" << std::endl;
    oss << "    const delay = config.delay;" << std::endl;
    oss << "    const callback = config.callback;" << std::endl;
    oss << std::endl;
    oss << "    if (config.begin) {" << std::endl;
    oss << "        Object.assign(element.style, config.begin);" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    if (config.when) {" << std::endl;
    oss << "        config.when.forEach(keyframe => {" << std::endl;
    oss << "            // Process keyframe" << std::endl;
    oss << "        });" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    if (config.end) {" << std::endl;
    oss << "        Object.assign(element.style, config.end);" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    if (callback) {" << std::endl;
    oss << "        callback();" << std::endl;
    oss << "    }" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

bool CHTLJSAnimationSystem::validateAnimation(const AnimationConfig& config) const {
    if (config.target.empty()) {
        return false;
    }
    
    if (config.duration <= 0) {
        return false;
    }
    
    return true;
}

std::vector<std::string> CHTLJSAnimationSystem::getValidationErrors(const AnimationConfig& config) const {
    std::vector<std::string> errors;
    
    if (config.target.empty()) {
        errors.push_back("Animation target is required");
    }
    
    if (config.duration <= 0) {
        errors.push_back("Animation duration must be positive");
    }
    
    return errors;
}

std::string CHTLJSAnimationSystem::generateTargetSelector(const std::string& target) const {
    if (target.find("{{") != std::string::npos) {
        return "document.querySelector('" + target + "')";
    }
    return target;
}

std::string CHTLJSAnimationSystem::generateEasingFunction(const std::string& easing) const {
    if (easing == "ease-in-out") {
        return "easeInOutQuad";
    } else if (easing == "ease-in") {
        return "easeInQuad";
    } else if (easing == "ease-out") {
        return "easeOutQuad";
    }
    return "linear";
}

std::string CHTLJSAnimationSystem::generateKeyframes(const std::vector<std::map<std::string, std::string>>& when) const {
    std::ostringstream oss;
    
    for (const auto& keyframe : when) {
        oss << "        {" << std::endl;
        for (const auto& pair : keyframe) {
            oss << "            " << pair.first << ": '" << pair.second << "'," << std::endl;
        }
        oss << "        }," << std::endl;
    }
    
    return oss.str();
}

std::string CHTLJSAnimationSystem::generateCallback(const std::string& callback) const {
    if (callback.empty()) {
        return "null";
    }
    return callback;
}

// CHTLJSRoutingSystem 实现
CHTLJSRoutingSystem::CHTLJSRoutingSystem() = default;

CHTLJSRoutingSystem::~CHTLJSRoutingSystem() = default;

void CHTLJSRoutingSystem::addRoute(const RouteConfig& route) {
    routes_.push_back(route);
}

void CHTLJSRoutingSystem::removeRoute(const std::string& path) {
    routes_.erase(
        std::remove_if(routes_.begin(), routes_.end(),
            [&path](const RouteConfig& route) {
                return route.path == path;
            }),
        routes_.end()
    );
}

std::string CHTLJSRoutingSystem::generateRouter() const {
    std::ostringstream oss;
    
    oss << "class Router {" << std::endl;
    oss << "    constructor() {" << std::endl;
    oss << "        this.routes = [];" << std::endl;
    oss << "        this.init();" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    init() {" << std::endl;
    oss << "        window.addEventListener('popstate', () => this.handleRoute());" << std::endl;
    oss << "        this.handleRoute();" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    addRoute(route) {" << std::endl;
    oss << "        this.routes.push(route);" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    handleRoute() {" << std::endl;
    oss << "        const path = window.location.pathname;" << std::endl;
    oss << "        const route = this.routes.find(r => this.matchRoute(r.path, path));" << std::endl;
    oss << "        if (route) {" << std::endl;
    oss << "            this.loadComponent(route.component);" << std::endl;
    oss << "        }" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    matchRoute(routePath, currentPath) {" << std::endl;
    oss << "        // Route matching logic" << std::endl;
    oss << "        return routePath === currentPath;" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    loadComponent(component) {" << std::endl;
    oss << "        // Component loading logic" << std::endl;
    oss << "    }" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

std::string CHTLJSRoutingSystem::generateRouteHandler(const RouteConfig& route) const {
    std::ostringstream oss;
    
    oss << "const route_" << route.path << " = {" << std::endl;
    oss << "    path: '" << route.path << "'," << std::endl;
    oss << "    component: '" << route.component << "'," << std::endl;
    oss << "    params: {" << std::endl;
    for (const auto& pair : route.params) {
        oss << "        " << pair.first << ": '" << pair.second << "'," << std::endl;
    }
    oss << "    }," << std::endl;
    oss << "    children: [" << std::endl;
    for (const auto& child : route.children) {
        oss << "        '" << child << "'," << std::endl;
    }
    oss << "    ]" << std::endl;
    oss << "};" << std::endl;
    
    return oss.str();
}

std::vector<CHTLJSRoutingSystem::RouteConfig> CHTLJSRoutingSystem::getRoutes() const {
    return routes_;
}

CHTLJSRoutingSystem::RouteConfig CHTLJSRoutingSystem::getRoute(const std::string& path) const {
    auto it = std::find_if(routes_.begin(), routes_.end(),
        [&path](const RouteConfig& route) {
            return route.path == path;
        });
    
    if (it != routes_.end()) {
        return *it;
    }
    
    return RouteConfig();
}

bool CHTLJSRoutingSystem::hasRoute(const std::string& path) const {
    return std::any_of(routes_.begin(), routes_.end(),
        [&path](const RouteConfig& route) {
            return route.path == path;
        });
}

bool CHTLJSRoutingSystem::validateRoute(const RouteConfig& route) const {
    return !route.path.empty() && !route.component.empty();
}

std::vector<std::string> CHTLJSRoutingSystem::getValidationErrors(const RouteConfig& route) const {
    std::vector<std::string> errors;
    
    if (route.path.empty()) {
        errors.push_back("Route path is required");
    }
    
    if (route.component.empty()) {
        errors.push_back("Route component is required");
    }
    
    return errors;
}

std::string CHTLJSRoutingSystem::generateRouteMatcher(const std::string& path) const {
    return "routePath === currentPath";
}

std::string CHTLJSRoutingSystem::generateParamExtractor(const std::map<std::string, std::string>& params) const {
    std::ostringstream oss;
    
    for (const auto& pair : params) {
        oss << "        " << pair.first << ": '" << pair.second << "'," << std::endl;
    }
    
    return oss.str();
}

std::string CHTLJSRoutingSystem::generateComponentLoader(const std::string& component) const {
    return "loadComponent('" + component + "')";
}

// CHTLJSReactiveSystem 实现
CHTLJSReactiveSystem::CHTLJSReactiveSystem() = default;

CHTLJSReactiveSystem::~CHTLJSReactiveSystem() = default;

void CHTLJSReactiveSystem::addReactiveValue(const ReactiveValue& value) {
    reactive_values_.push_back(value);
}

void CHTLJSReactiveSystem::removeReactiveValue(const std::string& name) {
    reactive_values_.erase(
        std::remove_if(reactive_values_.begin(), reactive_values_.end(),
            [&name](const ReactiveValue& value) {
                return value.name == name;
            }),
        reactive_values_.end()
    );
}

std::string CHTLJSReactiveSystem::generateReactiveSystem() const {
    std::ostringstream oss;
    
    oss << "class ReactiveSystem {" << std::endl;
    oss << "    constructor() {" << std::endl;
    oss << "        this.values = new Map();" << std::endl;
    oss << "        this.dependencies = new Map();" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    createReactiveValue(name, initialValue, type, dependencies) {" << std::endl;
    oss << "        const value = {" << std::endl;
    oss << "            name," << std::endl;
    oss << "            value: initialValue," << std::endl;
    oss << "            type," << std::endl;
    oss << "            dependencies" << std::endl;
    oss << "        };" << std::endl;
    oss << "        this.values.set(name, value);" << std::endl;
    oss << "        return value;" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    updateValue(name, newValue) {" << std::endl;
    oss << "        const value = this.values.get(name);" << std::endl;
    oss << "        if (value) {" << std::endl;
    oss << "            value.value = newValue;" << std::endl;
    oss << "            this.notifyDependencies(name);" << std::endl;
    oss << "        }" << std::endl;
    oss << "    }" << std::endl;
    oss << std::endl;
    oss << "    notifyDependencies(name) {" << std::endl;
    oss << "        // Notify dependent values" << std::endl;
    oss << "    }" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

std::string CHTLJSReactiveSystem::generateReactiveValue(const ReactiveValue& value) const {
    std::ostringstream oss;
    
    oss << "const " << value.name << " = reactiveSystem.createReactiveValue(" << std::endl;
    oss << "    '" << value.name << "'," << std::endl;
    oss << "    '" << value.initialValue << "'," << std::endl;
    oss << "    '" << value.type << "'," << std::endl;
    oss << "    [" << std::endl;
    for (const auto& dep : value.dependencies) {
        oss << "        '" << dep << "'," << std::endl;
    }
    oss << "    ]" << std::endl;
    oss << ");" << std::endl;
    
    return oss.str();
}

std::vector<CHTLJSReactiveSystem::ReactiveValue> CHTLJSReactiveSystem::getReactiveValues() const {
    return reactive_values_;
}

CHTLJSReactiveSystem::ReactiveValue CHTLJSReactiveSystem::getReactiveValue(const std::string& name) const {
    auto it = std::find_if(reactive_values_.begin(), reactive_values_.end(),
        [&name](const ReactiveValue& value) {
            return value.name == name;
        });
    
    if (it != reactive_values_.end()) {
        return *it;
    }
    
    return ReactiveValue();
}

bool CHTLJSReactiveSystem::hasReactiveValue(const std::string& name) const {
    return std::any_of(reactive_values_.begin(), reactive_values_.end(),
        [&name](const ReactiveValue& value) {
            return value.name == name;
        });
}

bool CHTLJSReactiveSystem::validateReactiveValue(const ReactiveValue& value) const {
    return !value.name.empty() && !value.type.empty();
}

std::vector<std::string> CHTLJSReactiveSystem::getValidationErrors(const ReactiveValue& value) const {
    std::vector<std::string> errors;
    
    if (value.name.empty()) {
        errors.push_back("Reactive value name is required");
    }
    
    if (value.type.empty()) {
        errors.push_back("Reactive value type is required");
    }
    
    return errors;
}

std::string CHTLJSReactiveSystem::generateReactiveProxy(const ReactiveValue& value) const {
    std::ostringstream oss;
    
    oss << "const " << value.name << " = new Proxy({" << std::endl;
    oss << "    value: '" << value.initialValue << "'" << std::endl;
    oss << "}, {" << std::endl;
    oss << "    set(target, property, newValue) {" << std::endl;
    oss << "        target[property] = newValue;" << std::endl;
    oss << "        // Notify dependencies" << std::endl;
    oss << "        return true;" << std::endl;
    oss << "    }" << std::endl;
    oss << "});" << std::endl;
    
    return oss.str();
}

std::string CHTLJSReactiveSystem::generateDependencyTracker(const std::vector<std::string>& dependencies) const {
    std::ostringstream oss;
    
    oss << "const dependencies = [" << std::endl;
    for (const auto& dep : dependencies) {
        oss << "    '" << dep << "'," << std::endl;
    }
    oss << "];" << std::endl;
    
    return oss.str();
}

std::string CHTLJSReactiveSystem::generateUpdateHandler(const ReactiveValue& value) const {
    std::ostringstream oss;
    
    oss << "function update" << value.name << "(newValue) {" << std::endl;
    oss << "    " << value.name << ".value = newValue;" << std::endl;
    oss << "    // Notify dependencies" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

// CHTLJSCompiler 实现
CHTLJSCompiler::CHTLJSCompiler() = default;

CHTLJSCompiler::~CHTLJSCompiler() = default;

void CHTLJSCompiler::setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager) {
    config_manager_ = configManager;
}

std::shared_ptr<ConfigurationManager> CHTLJSCompiler::getConfigurationManager() const {
    return config_manager_;
}

std::string CHTLJSCompiler::compile(const std::string& content) {
    std::string result = content;
    
    // 处理文件载入器
    result = processFileLoader(result);
    
    // 处理增强选择器
    result = processEnhancedSelectors(result);
    
    // 处理动画
    result = processAnimations(result);
    
    // 处理路由
    result = processRouting(result);
    
    // 处理响应式值
    result = processReactiveValues(result);
    
    return result;
}

std::string CHTLJSCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return compile(oss.str());
}

std::string CHTLJSCompiler::compileFiles(const std::vector<std::string>& filePaths) {
    std::ostringstream oss;
    
    for (const auto& filePath : filePaths) {
        std::string content = compileFile(filePath);
        if (!content.empty()) {
            oss << "// File: " << filePath << std::endl;
            oss << content << std::endl;
            oss << std::endl;
        }
    }
    
    return oss.str();
}

void CHTLJSCompiler::addComponent(const std::string& name, const std::string& content) {
    components_[name] = content;
}

void CHTLJSCompiler::removeComponent(const std::string& name) {
    components_.erase(name);
}

std::string CHTLJSCompiler::getComponent(const std::string& name) const {
    auto it = components_.find(name);
    if (it != components_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSCompiler::hasComponent(const std::string& name) const {
    return components_.find(name) != components_.end();
}

void CHTLJSCompiler::addModule(const std::string& name, const std::string& content) {
    modules_[name] = content;
}

void CHTLJSCompiler::removeModule(const std::string& name) {
    modules_.erase(name);
}

std::string CHTLJSCompiler::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSCompiler::hasModule(const std::string& name) const {
    return modules_.find(name) != modules_.end();
}

bool CHTLJSCompiler::validate(const std::string& content) const {
    // 基本验证
    if (content.empty()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> CHTLJSCompiler::getValidationErrors(const std::string& content) const {
    std::vector<std::string> errors;
    
    if (content.empty()) {
        errors.push_back("Content cannot be empty");
    }
    
    return errors;
}

void CHTLJSCompiler::clear() {
    clearComponents();
    clearModules();
}

void CHTLJSCompiler::clearComponents() {
    components_.clear();
}

void CHTLJSCompiler::clearModules() {
    modules_.clear();
}

std::string CHTLJSCompiler::processFileLoader(const std::string& content) const {
    std::regex fileLoaderRegex(R"(fileloader\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, fileLoaderRegex)) {
        std::string fileLoaderContent = match[1].str();
        return parseFileLoaderBlock(fileLoaderContent);
    }
    
    return content;
}

std::string CHTLJSCompiler::processEnhancedSelectors(const std::string& content) const {
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::string result = content;
    
    std::sregex_iterator begin(content.begin(), content.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string selector = it->str();
        std::string cleanSelector = selector.substr(2, selector.length() - 4);
        std::string jsCode = enhanced_selector_.generateJavaScript(cleanSelector);
        result = std::regex_replace(result, std::regex(selector), jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processAnimations(const std::string& content) const {
    std::regex animationRegex(R"(animate\s*\{([^}]+)\})");
    std::string result = content;
    
    std::sregex_iterator begin(content.begin(), content.end(), animationRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string animation = it->str();
        std::string cleanAnimation = animation.substr(7, animation.length() - 8);
        std::string jsCode = parseAnimationBlock(cleanAnimation);
        result = std::regex_replace(result, std::regex(animation), jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processRouting(const std::string& content) const {
    std::regex routeRegex(R"(route\s*\{([^}]+)\})");
    std::string result = content;
    
    std::sregex_iterator begin(content.begin(), content.end(), routeRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string route = it->str();
        std::string cleanRoute = route.substr(6, route.length() - 7);
        std::string jsCode = parseRouteBlock(cleanRoute);
        result = std::regex_replace(result, std::regex(route), jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processReactiveValues(const std::string& content) const {
    std::regex reactiveRegex(R"(\$(\w+)\$)");
    std::string result = content;
    
    std::sregex_iterator begin(content.begin(), content.end(), reactiveRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string reactive = it->str();
        std::string varName = reactive.substr(1, reactive.length() - 2);
        std::string jsCode = "reactiveSystem.values.get('" + varName + "').value";
        result = std::regex_replace(result, std::regex(reactive), jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::parseFileLoaderBlock(const std::string& content) const {
    std::vector<std::string> files;
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.find("load:") != std::string::npos) {
            std::string filePath = line.substr(line.find(":") + 1);
            filePath = filePath.substr(0, filePath.find(","));
            files.push_back(filePath);
        }
    }
    
    return generateFileLoaderCode(files);
}

std::string CHTLJSCompiler::parseEnhancedSelector(const std::string& content) const {
    return enhanced_selector_.generateJavaScript(content);
}

std::string CHTLJSCompiler::parseAnimationBlock(const std::string& content) const {
    CHTLJSAnimationSystem::AnimationConfig config;
    // 解析动画配置
    return animation_system_.generateAnimation(config);
}

std::string CHTLJSCompiler::parseRouteBlock(const std::string& content) const {
    CHTLJSRoutingSystem::RouteConfig config;
    // 解析路由配置
    return routing_system_.generateRouteHandler(config);
}

std::string CHTLJSCompiler::parseReactiveValue(const std::string& content) const {
    CHTLJSReactiveSystem::ReactiveValue value;
    // 解析响应式值
    return reactive_system_.generateReactiveValue(value);
}

std::string CHTLJSCompiler::generateFileLoaderCode(const std::vector<std::string>& files) const {
    std::ostringstream oss;
    
    oss << "// File loader" << std::endl;
    for (const auto& file : files) {
        oss << "loadFile('" << file << "');" << std::endl;
    }
    
    return oss.str();
}

std::string CHTLJSCompiler::generateEnhancedSelectorCode(const std::string& selector) const {
    return enhanced_selector_.generateJavaScript(selector);
}

std::string CHTLJSCompiler::generateAnimationCode(const std::string& animation) const {
    return animation_system_.generateAnimationFunction(CHTLJSAnimationSystem::AnimationConfig());
}

std::string CHTLJSCompiler::generateRouteCode(const std::string& route) const {
    return routing_system_.generateRouter();
}

std::string CHTLJSCompiler::generateReactiveCode(const std::string& reactive) const {
    return reactive_system_.generateReactiveSystem();
}

} // namespace CHTL