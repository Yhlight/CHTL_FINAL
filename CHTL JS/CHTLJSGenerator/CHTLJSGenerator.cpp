#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/CHTLJSFunctionNode.h"
#include "../CHTLJSNode/CHTLJSSpecialNode.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator(std::shared_ptr<CHTLJSContext> context)
    : context_(context), indentation_level_(0) {
    updateIndentation();
}

CHTLJSGenerator::~CHTLJSGenerator() {
}

std::string CHTLJSGenerator::generate(std::shared_ptr<CHTLJSBaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    switch (ast->getType()) {
        case CHTLJSBaseNode::NodeType::SCRIPT_LOADER:
            return generateScriptLoader(ast);
        case CHTLJSBaseNode::NodeType::LISTEN:
            return generateListen(ast);
        case CHTLJSBaseNode::NodeType::ANIMATE:
            return generateAnimate(ast);
        case CHTLJSBaseNode::NodeType::ROUTER:
            return generateRouter(ast);
        case CHTLJSBaseNode::NodeType::VIR:
            return generateVir(ast);
        case CHTLJSBaseNode::NodeType::INEVERAWAY:
            return generateINeverAway(ast);
        case CHTLJSBaseNode::NodeType::UTIL_THEN:
            return generateUtilThen(ast);
        case CHTLJSBaseNode::NodeType::PRINTMYLOVE:
            return generatePrintMylove(ast);
        case CHTLJSBaseNode::NodeType::SELECTOR:
            return generateSelector(ast);
        case CHTLJSBaseNode::NodeType::RESPONSIVE_VALUE:
            return generateResponsiveValue(ast);
        case CHTLJSBaseNode::NodeType::BLOCK:
            return generateBlock(ast);
        default:
            return ast->generateCode();
    }
}

std::string CHTLJSGenerator::generateScriptLoader(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS ScriptLoader implementation\n";
    ss << "(function() {\n";
    increaseIndentation();
    
    ss << getIndentation() << "const modules = {};\n";
    ss << getIndentation() << "const loadedModules = new Set();\n\n";
    
    // 生成加载逻辑
    const auto& stringParams = funcNode->getStringParameters();
    if (!stringParams.empty()) {
        ss << getIndentation() << "const loadModules = [\n";
        increaseIndentation();
        
        for (const auto& param : stringParams) {
            ss << getIndentation() << "\"" << escapeString(param.second) << "\",\n";
        }
        
        decreaseIndentation();
        ss << getIndentation() << "];\n\n";
        
        ss << getIndentation() << "loadModules.forEach(function(modulePath) {\n";
        increaseIndentation();
        ss << getIndentation() << "if (!loadedModules.has(modulePath)) {\n";
        increaseIndentation();
        ss << getIndentation() << "loadedModules.add(modulePath);\n";
        ss << getIndentation() << "const script = document.createElement('script');\n";
        ss << getIndentation() << "script.src = modulePath;\n";
        ss << getIndentation() << "script.async = true;\n";
        ss << getIndentation() << "document.head.appendChild(script);\n";
        decreaseIndentation();
        ss << getIndentation() << "}\n";
        decreaseIndentation();
        ss << getIndentation() << "});\n";
    }
    
    decreaseIndentation();
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateListen(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS Listen implementation\n";
    
    const auto& stringParams = funcNode->getStringParameters();
    const auto& nodeParams = funcNode->getNodeParams();
    
    // 生成事件监听器代码
    for (const auto& param : stringParams) {
        ss << "element.addEventListener('" << param.first << "', " << param.second << ");\n";
    }
    
    for (const auto& param : nodeParams) {
        ss << "element.addEventListener('" << param.first << "', " << generate(param.second) << ");\n";
    }
    
    return ss.str();
}

std::string CHTLJSGenerator::generateAnimate(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS Animate implementation\n";
    ss << "(function() {\n";
    increaseIndentation();
    
    ss << getIndentation() << "const target = " << generateSelector(node) << ";\n";
    ss << getIndentation() << "const duration = " << (funcNode->getStringParameters().count("duration") ? 
        funcNode->getStringParameters().at("duration") : "1000") << ";\n";
    ss << getIndentation() << "const easing = '" << (funcNode->getStringParameters().count("easing") ? 
        funcNode->getStringParameters().at("easing") : "ease-in-out") << "';\n\n";
    
    ss << getIndentation() << "function animate() {\n";
    increaseIndentation();
    ss << getIndentation() << "const startTime = performance.now();\n";
    ss << getIndentation() << "function frame(currentTime) {\n";
    increaseIndentation();
    ss << getIndentation() << "const elapsed = currentTime - startTime;\n";
    ss << getIndentation() << "const progress = Math.min(elapsed / duration, 1);\n";
    ss << getIndentation() << "// Apply animation properties here\n";
    ss << getIndentation() << "if (progress < 1) {\n";
    increaseIndentation();
    ss << getIndentation() << "requestAnimationFrame(frame);\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    ss << getIndentation() << "requestAnimationFrame(frame);\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    ss << getIndentation() << "animate();\n";
    
    decreaseIndentation();
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateRouter(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS Router implementation\n";
    ss << "(function() {\n";
    increaseIndentation();
    
    ss << getIndentation() << "const routes = {};\n";
    ss << getIndentation() << "const currentRoute = window.location.pathname;\n\n";
    
    ss << getIndentation() << "function navigate(path) {\n";
    increaseIndentation();
    ss << getIndentation() << "if (routes[path]) {\n";
    increaseIndentation();
    ss << getIndentation() << "routes[path]();\n";
    ss << getIndentation() << "window.history.pushState({}, '', path);\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n\n";
    
    ss << getIndentation() << "window.addEventListener('popstate', function() {\n";
    increaseIndentation();
    ss << getIndentation() << "navigate(window.location.pathname);\n";
    decreaseIndentation();
    ss << getIndentation() << "});\n";
    
    decreaseIndentation();
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateVir(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS Vir implementation\n";
    ss << "const " << funcNode->getFunctionName() << " = " << generate(funcNode->getNodeParameters().begin()->second) << ";\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS iNeverAway implementation\n";
    ss << "const iNeverAwayFunctions = {\n";
    increaseIndentation();
    
    const auto& stringParams = funcNode->getStringParameters();
    bool first = true;
    for (const auto& param : stringParams) {
        if (!first) ss << ",\n";
        ss << getIndentation() << param.first << ": " << param.second;
        first = false;
    }
    
    decreaseIndentation();
    ss << "\n};\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateUtilThen(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS util...then implementation\n";
    
    // 生成util表达式
    if (funcNode->getNodeParameters().count("expression")) {
        ss << "const utilExpression = " << generate(funcNode->getNodeParameters().at("expression")) << ";\n";
    }
    
    // 生成change块
    if (funcNode->getNodeParameters().count("change")) {
        ss << "function onChange() {\n";
        increaseIndentation();
        ss << getIndentation() << generate(funcNode->getNodeParameters().at("change"));
        decreaseIndentation();
        ss << "}\n";
    }
    
    // 生成then块
    if (funcNode->getNodeParameters().count("then")) {
        ss << "function onThen() {\n";
        increaseIndentation();
        ss << getIndentation() << generate(funcNode->getNodeParameters().at("then"));
        decreaseIndentation();
        ss << "}\n";
    }
    
    return ss.str();
}

std::string CHTLJSGenerator::generatePrintMylove(std::shared_ptr<CHTLJSBaseNode> node) {
    auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(node);
    if (!funcNode) {
        return "";
    }
    
    std::stringstream ss;
    ss << "// CHTL JS printMylove implementation\n";
    ss << "function printMylove(options) {\n";
    increaseIndentation();
    
    ss << getIndentation() << "const url = options.url;\n";
    ss << getIndentation() << "const mode = options.mode || 'ASCII';\n";
    ss << getIndentation() << "const width = options.width || 80;\n";
    ss << getIndentation() << "const height = options.height || 40;\n";
    ss << getIndentation() << "const scale = options.scale || 1;\n\n";
    
    ss << getIndentation() << "const img = new Image();\n";
    ss << getIndentation() << "img.onload = function() {\n";
    increaseIndentation();
    ss << getIndentation() << "const canvas = document.createElement('canvas');\n";
    ss << getIndentation() << "const ctx = canvas.getContext('2d');\n";
    ss << getIndentation() << "canvas.width = width;\n";
    ss << getIndentation() << "canvas.height = height;\n";
    ss << getIndentation() << "ctx.drawImage(img, 0, 0, width, height);\n";
    ss << getIndentation() << "const imageData = ctx.getImageData(0, 0, width, height);\n";
    ss << getIndentation() << "let result = '';\n";
    ss << getIndentation() << "for (let y = 0; y < height; y++) {\n";
    increaseIndentation();
    ss << getIndentation() << "for (let x = 0; x < width; x++) {\n";
    increaseIndentation();
    ss << getIndentation() << "const pixelIndex = (y * width + x) * 4;\n";
    ss << getIndentation() << "const r = imageData.data[pixelIndex];\n";
    ss << getIndentation() << "const g = imageData.data[pixelIndex + 1];\n";
    ss << getIndentation() << "const b = imageData.data[pixelIndex + 2];\n";
    ss << getIndentation() << "const brightness = (r + g + b) / 3;\n";
    ss << getIndentation() << "const char = brightness > 200 ? ' ' : brightness > 150 ? '.' : brightness > 100 ? ':' : brightness > 50 ? 'o' : '#';\n";
    ss << getIndentation() << "result += char;\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    ss << getIndentation() << "result += '\\n';\n";
    decreaseIndentation();
    ss << getIndentation() << "}\n";
    ss << getIndentation() << "console.log(result);\n";
    decreaseIndentation();
    ss << getIndentation() << "};\n";
    ss << getIndentation() << "img.src = url;\n";
    
    decreaseIndentation();
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateSelector(std::shared_ptr<CHTLJSBaseNode> node) {
    auto selectorNode = std::dynamic_pointer_cast<SelectorNode>(node);
    if (!selectorNode) {
        return "";
    }
    
    return selectorNode->generateCode();
}

std::string CHTLJSGenerator::generateResponsiveValue(std::shared_ptr<CHTLJSBaseNode> node) {
    auto responsiveNode = std::dynamic_pointer_cast<ResponsiveValueNode>(node);
    if (!responsiveNode) {
        return "";
    }
    
    return responsiveNode->generateCode();
}

std::string CHTLJSGenerator::generateBlock(std::shared_ptr<CHTLJSBaseNode> node) {
    auto blockNode = std::dynamic_pointer_cast<BlockNode>(node);
    if (!blockNode) {
        return "";
    }
    
    return blockNode->generateCode();
}

void CHTLJSGenerator::setIndentation(int level) {
    indentation_level_ = level;
    updateIndentation();
}

std::string CHTLJSGenerator::getIndentation() const {
    return indentation_string_;
}

void CHTLJSGenerator::increaseIndentation() {
    indentation_level_++;
    updateIndentation();
}

void CHTLJSGenerator::decreaseIndentation() {
    if (indentation_level_ > 0) {
        indentation_level_--;
        updateIndentation();
    }
}

void CHTLJSGenerator::updateIndentation() {
    indentation_string_ = std::string(indentation_level_ * 4, ' ');
}

std::string CHTLJSGenerator::escapeString(const std::string& str) {
    std::string result = str;
    std::replace(result.begin(), result.end(), '"', '\\"');
    std::replace(result.begin(), result.end(), '\n', '\\n');
    std::replace(result.begin(), result.end(), '\t', '\\t');
    std::replace(result.begin(), result.end(), '\r', '\\r');
    return result;
}

} // namespace CHTLJS