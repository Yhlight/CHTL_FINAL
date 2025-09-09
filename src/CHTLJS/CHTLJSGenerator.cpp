#include "CHTLJS/CHTLJSGenerator.h"
#include "CHTLJS/CHTLJSParser.h"
#include <sstream>

namespace CHTL {

CHTLJSGenerator::CHTLJSGenerator() {
}

CHTLJSGenerator::~CHTLJSGenerator() = default;

std::string CHTLJSGenerator::generateJavaScript(const std::string& chtljsCode) {
    CHTLJSParser parser;
    return parser.parse(chtljsCode);
}

std::string CHTLJSGenerator::generateFileLoader(const std::string& path) {
    std::stringstream result;
    result << "function loadFile(path) {\n";
    result << "    return fetch(path)\n";
    result << "        .then(response => {\n";
    result << "            if (!response.ok) {\n";
    result << "                throw new Error('Failed to load file: ' + path);\n";
    result << "            }\n";
    result << "            return response.text();\n";
    result << "        })\n";
    result << "        .catch(error => {\n";
    result << "            console.error('Error loading file:', error);\n";
    result << "            throw error;\n";
    result << "        });\n";
    return result.str();
}

std::string CHTLJSGenerator::generateScriptBlock(const std::string& content) {
    std::stringstream result;
    result << "(function() {\n";
    result << "    " << content << "\n";
    result << "})();\n";
    return result.str();
}

std::string CHTLJSGenerator::generateListener(const std::string& selector, const std::string& event, const std::string& handler) {
    std::stringstream result;
    result << "document.querySelector('" << selector << "').addEventListener('" << event << "', " << handler << ");\n";
    return result.str();
}

std::string CHTLJSGenerator::generateDelegate(const std::string& parentSelector, const std::string& targetSelector, 
                                            const std::string& event, const std::string& handler) {
    std::stringstream result;
    result << "document.querySelector('" << parentSelector << "').addEventListener('" << event << "', function(e) {\n";
    result << "    if (e.target.matches('" << targetSelector << "')) {\n";
    result << "        " << handler << "(e);\n";
    result << "    }\n";
    result << "});\n";
    return result.str();
}

std::string CHTLJSGenerator::generateAnimation(const std::string& name, const std::string& keyframes) {
    std::stringstream result;
    result << "const " << name << " = {\n";
    result << "    keyframes: " << keyframes << ",\n";
    result << "    duration: 1000,\n";
    result << "    easing: 'ease-in-out',\n";
    result << "    play: function(element) {\n";
    result << "        element.animate(this.keyframes, {\n";
    result << "            duration: this.duration,\n";
    result << "            easing: this.easing\n";
    result << "        });\n";
    result << "    }\n";
    result << "};\n";
    return result.str();
}

std::string CHTLJSGenerator::generateVirtualObject(const std::string& name, const std::string& definition) {
    std::stringstream result;
    result << "const " << name << " = {\n";
    result << "    " << definition << "\n";
    result << "};\n";
    return result.str();
}

std::string CHTLJSGenerator::generateRouter(const std::string& routes) {
    std::stringstream result;
    result << "const router = {\n";
    result << "    routes: " << routes << ",\n";
    result << "    currentRoute: '',\n";
    result << "    navigate: function(path) {\n";
    result << "        this.currentRoute = path;\n";
    result << "        const route = this.routes[path];\n";
    result << "        if (route) {\n";
    result << "            route();\n";
    result << "        }\n";
    result << "    },\n";
    result << "    init: function() {\n";
    result << "        window.addEventListener('popstate', () => {\n";
    result << "            this.navigate(window.location.pathname);\n";
    result << "        });\n";
    result << "        this.navigate(window.location.pathname);\n";
    result << "    }\n";
    result << "};\n";
    return result.str();
}

std::string CHTLJSGenerator::generateSelector(const std::string& selector) {
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::generateArrowFunction(const std::string& params, const std::string& body) {
    return "(" + params + ") => " + body;
}

} // namespace CHTL