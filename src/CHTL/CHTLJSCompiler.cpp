#include "CHTL/CHTLJS.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// CHTLJSCompiler 实现
CHTLJSCompiler::CHTLJSCompiler(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_fileLoader = std::make_unique<CHTLJSFileLoader>(context);
    m_selector = std::make_unique<CHTLJSSelector>(context);
    m_eventListener = std::make_unique<CHTLJSEventListener>(context);
    m_animation = std::make_unique<CHTLJSAnimation>(context);
    m_virtualObject = std::make_unique<CHTLJSVirtualObject>(context);
    m_router = std::make_unique<CHTLJSRouter>(context);
}

std::string CHTLJSCompiler::compileCHTLJS(const std::string& source) const {
    std::string result = source;
    
    // 处理文件加载器
    result = processFileLoader(result);
    
    // 处理选择器
    result = processSelectors(result);
    
    // 处理事件监听器
    result = processEventListeners(result);
    
    // 处理动画
    result = processAnimations(result);
    
    // 处理虚对象
    result = processVirtualObjects(result);
    
    // 处理路由
    result = processRouter(result);
    
    return result;
}

std::string CHTLJSCompiler::compileFile(const std::string& filePath) const {
    std::string source = m_fileLoader->loadFile(filePath);
    if (source.empty()) {
        return "";
    }
    
    return compileCHTLJS(source);
}

std::shared_ptr<BaseNode> CHTLJSCompiler::parseCHTLJS(const std::string& source) const {
    // 这里需要实现 CHTL JS 的语法解析
    // 暂时返回 nullptr，后续实现
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> CHTLJSCompiler::parseCHTLJSFile(const std::string& filePath) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    
    std::string source = m_fileLoader->loadFile(filePath);
    if (source.empty()) {
        return result;
    }
    
    auto ast = parseCHTLJS(source);
    if (ast) {
        result.push_back(ast);
    }
    
    return result;
}

std::string CHTLJSCompiler::generateJavaScript(const std::shared_ptr<BaseNode>& ast) const {
    if (!ast) {
        return "";
    }
    
    // 这里需要实现 AST 到 JavaScript 的转换
    // 暂时返回空字符串，后续实现
    return "";
}

std::string CHTLJSCompiler::generateModuleCode(const std::vector<std::shared_ptr<BaseNode>>& nodes) const {
    std::stringstream ss;
    
    ss << "// CHTL JS Module\n";
    ss << "(function() {\n";
    ss << "  'use strict';\n\n";
    
    for (const auto& node : nodes) {
        std::string jsCode = generateJavaScript(node);
        if (!jsCode.empty()) {
            ss << jsCode << "\n\n";
        }
    }
    
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCompiler::optimizeCode(const std::string& code) const {
    std::string result = code;
    
    // 基本的代码优化
    // 去除多余的空行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    result = std::regex_replace(result, multipleNewlines, "\n\n");
    
    // 去除多余的空格
    std::regex multipleSpaces(R"(\s{2,})");
    result = std::regex_replace(result, multipleSpaces, " ");
    
    return result;
}

std::string CHTLJSCompiler::minifyCode(const std::string& code) const {
    std::string result = code;
    
    // 基本的代码压缩
    // 去除注释
    std::regex commentRegex(R"(//.*$|/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 去除多余的空格和换行
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 去除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonRegex, ";");
    
    return result;
}

std::string CHTLJSCompiler::processFileLoader(const std::string& content) const {
    std::string result = content;
    
    // 处理 fileloader 语法
    std::regex fileloaderRegex(R"(fileloader\s*\{\s*load\s*:\s*([^}]+)\s*\})");
    std::smatch match;
    
    while (std::regex_search(result, match, fileloaderRegex)) {
        std::string loadPath = match[1].str();
        // 去除引号
        loadPath.erase(std::remove(loadPath.begin(), loadPath.end(), '"'), loadPath.end());
        loadPath.erase(std::remove(loadPath.begin(), loadPath.end(), '\''), loadPath.end());
        
        // 生成 AMD 风格的加载代码
        std::string amdCode = "define(['" + loadPath + "'], function(module) {\n" +
                             "  return module;\n" +
                             "});";
        
        result = std::regex_replace(result, fileloaderRegex, amdCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processSelectors(const std::string& content) const {
    std::string result = content;
    
    // 处理 {{CSS选择器}} 语法
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    while (std::regex_search(result, match, selectorRegex)) {
        std::string selector = match[1].str();
        std::string jsCode = m_selector->generateSelectorCode(selector);
        
        result = std::regex_replace(result, selectorRegex, jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processEventListeners(const std::string& content) const {
    std::string result = content;
    
    // 处理 listen 语法
    std::regex listenRegex(R"(listen\s*\{\s*([^}]+)\s*\})");
    std::smatch match;
    
    while (std::regex_search(result, match, listenRegex)) {
        std::string listeners = match[1].str();
        auto listenerPairs = m_eventListener->parseMultipleListeners(listeners);
        std::string jsCode = m_eventListener->generateMultipleListenerCode(listenerPairs);
        
        result = std::regex_replace(result, listenRegex, jsCode);
    }
    
    // 处理 delegate 语法
    std::regex delegateRegex(R"(delegate\s*\{\s*([^}]+)\s*\})");
    
    while (std::regex_search(result, match, delegateRegex)) {
        std::string delegation = match[1].str();
        std::string jsCode = m_eventListener->parseEventDelegation(delegation);
        
        result = std::regex_replace(result, delegateRegex, jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processAnimations(const std::string& content) const {
    std::string result = content;
    
    // 处理 animate 语法
    std::regex animateRegex(R"(animate\s*\{\s*([^}]+)\s*\})");
    std::smatch match;
    
    while (std::regex_search(result, match, animateRegex)) {
        std::string animationConfig = match[1].str();
        std::string jsCode = m_animation->generateAnimationCode(animationConfig);
        
        result = std::regex_replace(result, animateRegex, jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processVirtualObjects(const std::string& content) const {
    std::string result = content;
    
    // 处理 vir 语法
    std::regex virRegex(R"(vir\s+(\w+)\s*=\s*function\s*\{\s*([^}]+)\s*\})");
    std::smatch match;
    
    while (std::regex_search(result, match, virRegex)) {
        std::string objectName = match[1].str();
        std::string function = match[2].str();
        std::string jsCode = m_virtualObject->generateVirtualObjectCode(objectName, function);
        
        result = std::regex_replace(result, virRegex, jsCode);
    }
    
    return result;
}

std::string CHTLJSCompiler::processRouter(const std::string& content) const {
    std::string result = content;
    
    // 处理 router 语法
    std::regex routerRegex(R"(router\s*\{\s*([^}]+)\s*\})");
    std::smatch match;
    
    while (std::regex_search(result, match, routerRegex)) {
        std::string routeConfig = match[1].str();
        std::string jsCode = m_router->generateRouterCode(routeConfig);
        
        result = std::regex_replace(result, routerRegex, jsCode);
    }
    
    return result;
}

} // namespace CHTL