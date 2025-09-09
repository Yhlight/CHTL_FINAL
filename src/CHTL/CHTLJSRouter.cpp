#include "CHTL/CHTLJS.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSRouter 实现
CHTLJSRouter::CHTLJSRouter(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CHTLJSRouter::parseRoute(const std::string& routeConfig) const {
    auto properties = parseRouteProperties(routeConfig);
    return generateRouteHandler(properties);
}

std::unordered_map<std::string, std::string> CHTLJSRouter::parseRouteProperties(const std::string& config) const {
    std::unordered_map<std::string, std::string> properties;
    
    // 解析路由配置
    std::regex propertyRegex(R"(\s*(\w+)\s*:\s*([^,;]+)(?:[,;]|$))");
    std::sregex_iterator iter(config.begin(), config.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // 去除前后空格
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        properties[key] = value;
    }
    
    return properties;
}

std::string CHTLJSRouter::generateRouterCode(const std::string& routeConfig) const {
    auto properties = parseRouteProperties(routeConfig);
    return generateRouteHandler(properties);
}

std::string CHTLJSRouter::generateRouteHandler(const std::unordered_map<std::string, std::string>& properties) const {
    std::stringstream ss;
    
    ss << "class CHTLRouter {\n";
    ss << "  constructor() {\n";
    ss << "    this.routes = new Map();\n";
    ss << "    this.currentRoute = null;\n";
    ss << "    this.mode = '" << (properties.count("mode") ? properties.at("mode") : "hash") << "';\n";
    ss << "    this.root = '" << (properties.count("root") ? properties.at("root") : "/") << "';\n\n";
    
    ss << "    this.init();\n";
    ss << "  }\n\n";
    
    ss << "  init() {\n";
    ss << "    if (this.mode === 'hash') {\n";
    ss << "      window.addEventListener('hashchange', () => this.handleRoute());\n";
    ss << "    } else if (this.mode === 'history') {\n";
    ss << "      window.addEventListener('popstate', () => this.handleRoute());\n";
    ss << "    }\n";
    ss << "    this.handleRoute();\n";
    ss << "  }\n\n";
    
    ss << "  addRoute(url, page) {\n";
    ss << "    this.routes.set(url, page);\n";
    ss << "  }\n\n";
    
    ss << "  navigate(url) {\n";
    ss << "    if (this.mode === 'hash') {\n";
    ss << "      window.location.hash = url;\n";
    ss << "    } else if (this.mode === 'history') {\n";
    ss << "      window.history.pushState(null, null, this.root + url);\n";
    ss << "    }\n";
    ss << "    this.handleRoute();\n";
    ss << "  }\n\n";
    
    ss << "  handleRoute() {\n";
    ss << "    let url = '';\n";
    ss << "    if (this.mode === 'hash') {\n";
    ss << "      url = window.location.hash.slice(1);\n";
    ss << "    } else if (this.mode === 'history') {\n";
    ss << "      url = window.location.pathname.replace(this.root, '');\n";
    ss << "    }\n\n";
    
    ss << "    const page = this.routes.get(url);\n";
    ss << "    if (page) {\n";
    ss << "      this.currentRoute = url;\n";
    ss << "      this.renderPage(page);\n";
    ss << "    } else {\n";
    ss << "      this.render404();\n";
    ss << "    }\n";
    ss << "  }\n\n";
    
    ss << "  renderPage(page) {\n";
    ss << "    const container = document.querySelector('#app');\n";
    ss << "    if (container) {\n";
    ss << "      container.innerHTML = page;\n";
    ss << "    }\n";
    ss << "  }\n\n";
    
    ss << "  render404() {\n";
    ss << "    const container = document.querySelector('#app');\n";
    ss << "    if (container) {\n";
    ss << "      container.innerHTML = '<h1>404 - Page Not Found</h1>';\n";
    ss << "    }\n";
    ss << "  }\n";
    ss << "}\n\n";
    
    ss << "// 创建路由器实例\n";
    ss << "const router = new CHTLRouter();\n";
    
    return ss.str();
}

std::string CHTLJSRouter::generateRouteRegistration(const std::string& url, const std::string& page) const {
    if (!isValidRouteUrl(url)) {
        m_context->addError("Invalid route URL: " + url);
        return "";
    }
    
    std::stringstream ss;
    ss << "router.addRoute('" << url << "', `" << page << "`);\n";
    return ss.str();
}

std::string CHTLJSRouter::generateRouteNavigation(const std::string& url) const {
    if (!isValidRouteUrl(url)) {
        m_context->addError("Invalid route URL: " + url);
        return "";
    }
    
    std::stringstream ss;
    ss << "router.navigate('" << url << "');\n";
    return ss.str();
}

std::string CHTLJSRouter::generateHistoryManagement(const std::string& mode) const {
    if (mode == "hash") {
        return generateHashMode();
    } else if (mode == "history") {
        return generateHistoryMode();
    } else {
        m_context->addWarning("Unknown history mode: " + mode);
        return generateHashMode();
    }
}

std::string CHTLJSRouter::generateHashMode() const {
    std::stringstream ss;
    
    ss << "// Hash 模式路由\n";
    ss << "class HashRouter {\n";
    ss << "  constructor() {\n";
    ss << "    this.routes = new Map();\n";
    ss << "    window.addEventListener('hashchange', () => this.handleRoute());\n";
    ss << "    this.handleRoute();\n";
    ss << "  }\n\n";
    
    ss << "  addRoute(hash, handler) {\n";
    ss << "    this.routes.set(hash, handler);\n";
    ss << "  }\n\n";
    
    ss << "  navigate(hash) {\n";
    ss << "    window.location.hash = hash;\n";
    ss << "  }\n\n";
    
    ss << "  handleRoute() {\n";
    ss << "    const hash = window.location.hash.slice(1) || '/';\n";
    ss << "    const handler = this.routes.get(hash);\n";
    ss << "    if (handler) {\n";
    ss << "      handler();\n";
    ss << "    }\n";
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSRouter::generateHistoryMode() const {
    std::stringstream ss;
    
    ss << "// History 模式路由\n";
    ss << "class HistoryRouter {\n";
    ss << "  constructor(root = '/') {\n";
    ss << "    this.routes = new Map();\n";
    ss << "    this.root = root;\n";
    ss << "    window.addEventListener('popstate', () => this.handleRoute());\n";
    ss << "    this.handleRoute();\n";
    ss << "  }\n\n";
    
    ss << "  addRoute(path, handler) {\n";
    ss << "    this.routes.set(path, handler);\n";
    ss << "  }\n\n";
    
    ss << "  navigate(path) {\n";
    ss << "    window.history.pushState(null, null, this.root + path);\n";
    ss << "    this.handleRoute();\n";
    ss << "  }\n\n";
    
    ss << "  handleRoute() {\n";
    ss << "    const path = window.location.pathname.replace(this.root, '') || '/';\n";
    ss << "    const handler = this.routes.get(path);\n";
    ss << "    if (handler) {\n";
    ss << "      handler();\n";
    ss << "    }\n";
    ss << "  }\n";
    ss << "}\n";
    
    return ss.str();
}

bool CHTLJSRouter::isValidRouteUrl(const std::string& url) const {
    if (url.empty()) {
        return false;
    }
    
    // 基本的 URL 验证
    std::regex urlRegex(R"(^[/][a-zA-Z0-9/_-]*$)");
    return std::regex_match(url, urlRegex);
}

std::string CHTLJSRouter::generateRouteMatcher(const std::string& url) const {
    std::stringstream ss;
    
    ss << "function matchRoute(url) {\n";
    ss << "  const pattern = '" << url << "';\n";
    ss << "  const regex = new RegExp('^' + pattern.replace(/\\*/g, '.*') + '$');\n";
    ss << "  return regex.test(url);\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSRouter::generateRouteParams(const std::string& url) const {
    std::stringstream ss;
    
    ss << "function extractParams(url) {\n";
    ss << "  const pattern = '" << url << "';\n";
    ss << "  const paramNames = [];\n";
    ss << "  const paramRegex = /:([a-zA-Z0-9_]+)/g;\n";
    ss << "  let match;\n";
    ss << "  while ((match = paramRegex.exec(pattern)) !== null) {\n";
    ss << "    paramNames.push(match[1]);\n";
    ss << "  }\n";
    ss << "  return paramNames;\n";
    ss << "}\n";
    
    return ss.str();
}

} // namespace CHTL