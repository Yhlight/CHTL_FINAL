#include "RouterNode.hpp"
#include <sstream>

namespace CHTL {

RouterNode::RouterNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::ROUTER, line, column), mode_("hash") {}

void RouterNode::setRoot(const std::string& root) {
    root_ = root;
}

void RouterNode::setMode(const std::string& mode) {
    mode_ = mode;
}

void RouterNode::addRoute(const Route& route) {
    routes_.push_back(route);
}

void RouterNode::addRoute(const std::string& url, const std::string& selector) {
    routes_.push_back({url, selector});
}

std::string RouterNode::toJavaScript() const {
    std::ostringstream oss;
    
    oss << "// CHTL Router\n";
    oss << "(function() {\n";
    oss << "    const router = {\n";
    oss << "        mode: '" << mode_ << "',\n";
    oss << "        root: '" << root_ << "',\n";
    oss << "        routes: [\n";
    
    for (size_t i = 0; i < routes_.size(); ++i) {
        const auto& route = routes_[i];
        oss << "            { url: '" << route.url << "', selector: '" << route.selector << "' }";
        if (i < routes_.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "        ],\n";
    oss << "        currentRoute: null,\n";
    oss << "        rootElement: null\n";
    oss << "    };\n";
    oss << "    \n";
    oss << "    // Initialize router\n";
    oss << "    function initRouter() {\n";
    oss << "        router.rootElement = document.querySelector(router.root);\n";
    oss << "        if (!router.rootElement) {\n";
    oss << "            console.error('Router root element not found:', router.root);\n";
    oss << "            return;\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        // Listen for route changes\n";
    oss << "        if (router.mode === 'hash') {\n";
    oss << "            window.addEventListener('hashchange', handleRouteChange);\n";
    oss << "        } else if (router.mode === 'history') {\n";
    oss << "            window.addEventListener('popstate', handleRouteChange);\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        // Initial route\n";
    oss << "        handleRouteChange();\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // Handle route changes\n";
    oss << "    function handleRouteChange() {\n";
    oss << "        let currentPath;\n";
    oss << "        \n";
    oss << "        if (router.mode === 'hash') {\n";
    oss << "            currentPath = window.location.hash.slice(1) || '/';\n";
    oss << "        } else {\n";
    oss << "            currentPath = window.location.pathname;\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        const route = findRoute(currentPath);\n";
    oss << "        if (route) {\n";
    oss << "            switchToRoute(route);\n";
    oss << "        } else {\n";
    oss << "            console.warn('Route not found:', currentPath);\n";
    oss << "        }\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // Find matching route\n";
    oss << "    function findRoute(path) {\n";
    oss << "        return router.routes.find(route => {\n";
    oss << "            if (route.url === path) return true;\n";
    oss << "            // Simple wildcard matching\n";
    oss << "            if (route.url.includes('*')) {\n";
    oss << "                const pattern = route.url.replace(/\\*/g, '.*');\n";
    oss << "                return new RegExp('^' + pattern + '$').test(path);\n";
    oss << "            }\n";
    oss << "            return false;\n";
    oss << "        });\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // Switch to route\n";
    oss << "    function switchToRoute(route) {\n";
    oss << "        if (router.currentRoute === route) return;\n";
    oss << "        \n";
    oss << "        // Hide current content\n";
    oss << "        const currentElements = router.rootElement.children;\n";
    oss << "        for (let i = 0; i < currentElements.length; i++) {\n";
    oss << "            currentElements[i].style.display = 'none';\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        // Show new content\n";
    oss << "        const newElements = document.querySelectorAll(route.selector);\n";
    oss << "        for (let i = 0; i < newElements.length; i++) {\n";
    oss << "            newElements[i].style.display = '';\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        router.currentRoute = route;\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // Navigate to route\n";
    oss << "    function navigateTo(path) {\n";
    oss << "        if (router.mode === 'hash') {\n";
    oss << "            window.location.hash = path;\n";
    oss << "        } else {\n";
    oss << "            window.history.pushState({}, '', path);\n";
    oss << "            handleRouteChange();\n";
    oss << "        }\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // Public API\n";
    oss << "    window.CHTLRouter = {\n";
    oss << "        navigate: navigateTo,\n";
    oss << "        getCurrentRoute: () => router.currentRoute,\n";
    oss << "        getRoutes: () => router.routes\n";
    oss << "    };\n";
    oss << "    \n";
    oss << "    // Initialize when DOM is ready\n";
    oss << "    if (document.readyState === 'loading') {\n";
    oss << "        document.addEventListener('DOMContentLoaded', initRouter);\n";
    oss << "    } else {\n";
    oss << "        initRouter();\n";
    oss << "    }\n";
    oss << "})();\n";
    
    return oss.str();
}

std::string RouterNode::toString() const {
    std::ostringstream oss;
    oss << "RouterNode(mode=\"" << mode_ 
        << "\", root=\"" << root_ 
        << "\", routes=" << routes_.size() 
        << ", line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL