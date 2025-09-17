#include "CHTLJSGenerator.h"

#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/PlaceholderNode.h"
#include "../CHTLJSNode/SequenceNode.h"
#include "../CHTLJSNode/ValueNode.h"
#include "../CHTLJSNode/VirNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"


#include <stdexcept>
#include <iostream>
#include <sstream>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const CHTLJSNode& root, const std::map<std::string, std::string>& placeholder_map) {
    this->placeholder_map_ = &placeholder_map;
    return visit(&root);
}

std::string CHTLJSGenerator::visit(const CHTLJSNode* node) {
    if (!node) {
        return "";
    }

    switch (node->getType()) {
        case CHTLJSNodeType::Sequence:
            return visitSequenceNode(static_cast<const SequenceNode*>(node));
        case CHTLJSNodeType::Placeholder:
            return visitPlaceholderNode(static_cast<const PlaceholderNode*>(node));
        case CHTLJSNodeType::EnhancedSelector:
            return visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node));
        case CHTLJSNodeType::Listen:
            return visitListenNode(static_cast<const ListenNode*>(node));
        case CHTLJSNodeType::Value:
            return visitValueNode(static_cast<const ValueNode*>(node));
        case CHTLJSNodeType::Animate:
             return visitAnimateNode(static_cast<const AnimateNode*>(node));
        case CHTLJSNodeType::Delegate:
             return "/* DelegateNode not fully implemented */";
        case CHTLJSNodeType::Vir:
             return "/* VirNode not fully implemented */";
        case CHTLJSNodeType::Router:
            return visitRouterNode(static_cast<const RouterNode*>(node));
        case CHTLJSNodeType::ScriptLoader:
            return visitScriptLoaderNode(static_cast<const ScriptLoaderNode*>(node));
        default:
            throw std::runtime_error("Unknown CHTLJSNode type in generator.");
    }
}

std::string CHTLJSGenerator::visitScriptLoaderNode(const ScriptLoaderNode* node) {
    std::stringstream ss;
    ss << "(function() {\n";
    ss << "    const scripts = [\n";
    for (const auto& path : node->file_paths) {
        ss << "        '" << path << "',\n";
    }
    ss << "    ];\n\n";

    ss << R"__(
    function loadScript(index) {
        if (index >= scripts.length) {
            return; // All scripts loaded
        }
        const script = document.createElement('script');
        script.src = scripts[index];
        script.onload = () => {
            // console.log(`Loaded ${scripts[index]}`);
            loadScript(index + 1); // Load the next script
        };
        script.onerror = () => {
            console.error(`Failed to load ${scripts[index]}`);
            loadScript(index + 1); // Continue with next script even if one fails
        };
        document.head.appendChild(script);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', () => loadScript(0));
    } else {
        loadScript(0);
    }
)__";

    ss << "\n})();\n";
    return ss.str();
}

std::string CHTLJSGenerator::visitSequenceNode(const SequenceNode* node) {
    std::string result;
    for (const auto& statement : node->statements_) {
        result += visit(statement.get());
    }
    return result;
}

std::string CHTLJSGenerator::visitPlaceholderNode(const PlaceholderNode* node) {
    if (!placeholder_map_) {
        throw std::runtime_error("Placeholder map is not set in the generator.");
    }
    auto it = placeholder_map_->find(node->getPlaceholderText());
    if (it == placeholder_map_->end()) {
        // This should not happen if the scanner and parser are correct.
        throw std::runtime_error("Could not find placeholder in map: " + node->getPlaceholderText());
    }
    return it->second;
}

std::string CHTLJSGenerator::visitEnhancedSelector(const EnhancedSelectorNode* node) {
    // This is a simplified transformation. A real one would need more robust parsing.
    const std::string& selector = node->getSelector();
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::visitListenNode(const ListenNode* node) {
    std::string target_js = visit(node->getObject());
    std::string result;
    for (const auto& pair : node->getEvents()) {
        const std::string& event_name = pair.first;
        const std::string& callback_placeholder = pair.second;

        if (placeholder_map_->find(callback_placeholder) == placeholder_map_->end()) {
            throw std::runtime_error("Could not find callback placeholder in map: " + callback_placeholder);
        }
        std::string callback_js = placeholder_map_->at(callback_placeholder);

        result += target_js + ".addEventListener('" + event_name + "', " + callback_js + ");\n";
    }
    return result;
}

std::string CHTLJSGenerator::visitValueNode(const ValueNode* node) {
    return node->getValue();
}

std::string CHTLJSGenerator::visitAnimateNode(const AnimateNode* node) {
    std::stringstream ss;

    // 1. Keyframes
    ss << "const keyframes = [";
    for (const auto& kf : node->keyframes_) {
        ss << "{ ";
        if (kf.offset.has_value()) {
            ss << "offset: " << kf.offset.value() << ", ";
        }
        for (const auto& prop : kf.properties) {
            // CSS properties with dashes need to be camelCased for JS animation API
            std::string js_prop = prop.first;
            size_t dash_pos;
            while ((dash_pos = js_prop.find('-')) != std::string::npos) {
                js_prop.erase(dash_pos, 1);
                if (dash_pos < js_prop.length()) {
                    js_prop[dash_pos] = toupper(js_prop[dash_pos]);
                }
            }
            ss << js_prop << ": \"" << prop.second << "\", ";
        }
        ss << "},";
    }
    ss << "];\n";

    // 2. Options
    ss << "const options = {";
    if (node->duration_.has_value()) ss << "duration: " << node->duration_.value() << ",";
    if (node->easing_.has_value()) ss << "easing: \"" << node->easing_.value() << "\",";
    if (node->loop_.has_value()) ss << "iterations: " << node->loop_.value() << ",";
    if (node->direction_.has_value()) ss << "direction: \"" << node->direction_.value() << "\",";
    if (node->delay_.has_value()) ss << "delay: " << node->delay_.value() << ",";
    ss << "fill: 'forwards'};"; // Always fill forwards to maintain end state

    // 3. Targets and execution
    ss << "\n";
    for (const auto& target_node : node->targets_) {
        ss << visit(target_node.get()) << ".animate(keyframes, options);\n";
    }

    if (node->callback_.has_value()) {
        ss << "setTimeout(() => {" << placeholder_map_->at(node->callback_.value()) << "}, " << (node->duration_.has_value() ? node->duration_.value() : "0") << ");\n";
    }

    return ss.str();
}

std::string CHTLJSGenerator::visitRouterNode(const RouterNode* node) {
    std::stringstream ss;
    ss << "(function() {\n";

    // 1. Build routes map
    ss << "    const routes = {\n";
    for (const auto& route : node->routes) {
        // Assuming url is a ValueNode and page is an EnhancedSelectorNode
        ss << "        \"" << static_cast<ValueNode*>(route.url.get())->getValue() << "\": "
           << "document.querySelector(\"" << static_cast<EnhancedSelectorNode*>(route.page.get())->getSelector() << "\")" << ",\n";
    }
    ss << "    };\n";

    // 2. Get mode and root container
    std::string mode = node->mode.value_or("history");
    std::string container_selector = "document.body";
    if (node->root_config && node->root_config->container) {
        container_selector = "document.querySelector(\"" + static_cast<EnhancedSelectorNode*>(node->root_config->container.value().get())->getSelector() + "\")";
    }

    ss << "    const mode = '" << mode << "';\n";
    ss << "    const container = " << container_selector << ";\n";

    // 3. Navigation and rendering logic
    ss << R"__(
    function navigate(path) {
        if (mode === 'history') {
            history.pushState(null, null, path);
        } else {
            path = '#' + path;
            window.location.hash = path;
        }
        render(path);
    }

    function render(path) {
        if (mode === 'hash') {
            path = path.substring(1);
        }

        // Hide all pages within the container
        Object.values(routes).forEach(page => {
            if (page) page.style.display = 'none';
        });

        // Show the target page
        const targetPage = routes[path];
        if (targetPage) {
            targetPage.style.display = 'block';
        } else {
            // Optional: handle 404 - show a default page or do nothing
            const defaultPath = Object.keys(routes)[0] || '/';
            const defaultPage = routes[defaultPath];
            if(defaultPage) defaultPage.style.display = 'block';
        }
    }

    // 4. Event listeners
    if (mode === 'history') {
        window.addEventListener('popstate', () => render(window.location.pathname));
        document.addEventListener('click', e => {
            if (e.target.matches('a')) {
                const href = e.target.getAttribute('href');
                if (href && href.startsWith('/')) {
                    e.preventDefault();
                    navigate(href);
                }
            }
        });
    } else {
        window.addEventListener('hashchange', () => render(window.location.hash));
    }

    // 5. Initial render
    document.addEventListener('DOMContentLoaded', () => {
        const initialPath = mode === 'history' ? window.location.pathname : window.location.hash;
        render(initialPath || '/');
    });

)__";

    ss << "})();\n";
    return ss.str();
}


} // namespace CHTLJS
