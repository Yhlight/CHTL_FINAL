#include "CHTLJSCompiler.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CHTLJSCompiler::CHTLJSCompiler()
    : enableMinification_(false), enableSourceMap_(false) {
    context_ = std::make_shared<CHTLJSContext>();
}

std::string CHTLJSCompiler::compile(const std::string& source) {
    std::string result;
    
    // 预处理：处理占位符
    std::string processedSource = processPlaceholders(source);
    
    // 语法验证
    if (!isValidCHTLJSSyntax(processedSource)) {
        reportError("Invalid CHTL JS syntax");
        return "";
    }
    
    // 编译不同类型的代码块
    size_t pos = 0;
    while (pos < processedSource.length()) {
        // 查找CHTL JS关键字
        size_t virPos = processedSource.find("Vir", pos);
        size_t listenPos = processedSource.find("Listen", pos);
        size_t animatePos = processedSource.find("Animate", pos);
        size_t routerPos = processedSource.find("Router", pos);
        size_t scriptLoaderPos = processedSource.find("ScriptLoader", pos);
        size_t utilPos = processedSource.find("util", pos);
        
        // 找到最近的关键字
        size_t nextPos = std::string::npos;
        std::string keyword;
        
        if (virPos != std::string::npos && (nextPos == std::string::npos || virPos < nextPos)) {
            nextPos = virPos;
            keyword = "Vir";
        }
        if (listenPos != std::string::npos && (nextPos == std::string::npos || listenPos < nextPos)) {
            nextPos = listenPos;
            keyword = "Listen";
        }
        if (animatePos != std::string::npos && (nextPos == std::string::npos || animatePos < nextPos)) {
            nextPos = animatePos;
            keyword = "Animate";
        }
        if (routerPos != std::string::npos && (nextPos == std::string::npos || routerPos < nextPos)) {
            nextPos = routerPos;
            keyword = "Router";
        }
        if (scriptLoaderPos != std::string::npos && (nextPos == std::string::npos || scriptLoaderPos < nextPos)) {
            nextPos = scriptLoaderPos;
            keyword = "ScriptLoader";
        }
        if (utilPos != std::string::npos && (nextPos == std::string::npos || utilPos < nextPos)) {
            nextPos = utilPos;
            keyword = "util";
        }
        
        if (nextPos == std::string::npos) {
            // 没有找到CHTL JS关键字，添加剩余的普通JS代码
            result += processedSource.substr(pos);
            break;
        }
        
        // 添加普通JS代码
        if (nextPos > pos) {
            result += processedSource.substr(pos, nextPos - pos);
        }
        
        // 编译CHTL JS代码块
        size_t blockStart = nextPos;
        size_t blockEnd = blockStart;
        std::string blockContent = extractBlockContent(processedSource, blockStart, blockEnd);
        
        std::string compiledBlock;
        if (keyword == "Vir") {
            compiledBlock = compileVir(blockContent);
        } else if (keyword == "Listen") {
            compiledBlock = compileListen(blockContent);
        } else if (keyword == "Animate") {
            compiledBlock = compileAnimate(blockContent);
        } else if (keyword == "Router") {
            compiledBlock = compileRouter(blockContent);
        } else if (keyword == "ScriptLoader") {
            compiledBlock = compileScriptLoader(blockContent);
        } else if (keyword == "util") {
            compiledBlock = compileUtil(blockContent);
        }
        
        result += compiledBlock;
        pos = blockEnd;
    }
    
    // 后处理：代码优化
    if (enableMinification_) {
        result = minifyCode(result);
    }
    
    return result;
}

std::string CHTLJSCompiler::compileVir(const std::string& content) {
    // 编译虚对象
    std::string result = "// Virtual Object\n";
    result += "const virtualObject = {\n";
    
    // 解析虚对象内容
    std::string properties = extractObjectProperties(content);
    result += properties;
    
    result += "};\n";
    
    // 注册到上下文
    if (context_) {
        context_->registerVirtualObject("virtualObject", result);
    }
    
    return result;
}

std::string CHTLJSCompiler::compileListen(const std::string& content) {
    // 编译事件监听器
    std::string result = "// Event Listener\n";
    
    // 解析事件监听器内容
    std::string eventName = extractFunctionName(content);
    std::string handler = extractFunctionBody(content);
    
    result += "document.addEventListener('" + eventName + "', " + handler + ");\n";
    
    return result;
}

std::string CHTLJSCompiler::compileAnimate(const std::string& content) {
    // 编译动画
    std::string result = "// Animation\n";
    
    // 解析动画内容
    std::string animationName = extractFunctionName(content);
    std::string properties = extractObjectProperties(content);
    
    result += "const " + animationName + " = {\n";
    result += properties;
    result += "};\n";
    
    // 注册到上下文
    if (context_) {
        context_->registerAnimation(animationName, result);
    }
    
    return result;
}

std::string CHTLJSCompiler::compileRouter(const std::string& content) {
    // 编译路由器
    std::string result = "// Router\n";
    
    // 解析路由内容
    std::string routerName = extractFunctionName(content);
    std::string routes = extractObjectProperties(content);
    
    result += "const " + routerName + " = {\n";
    result += "  routes: {\n";
    result += routes;
    result += "  },\n";
    result += "  navigate: function(url) {\n";
    result += "    if (this.routes[url]) {\n";
    result += "      this.routes[url]();\n";
    result += "    }\n";
    result += "  }\n";
    result += "};\n";
    
    // 注册到上下文
    if (context_) {
        context_->registerRoute("default", routerName);
    }
    
    return result;
}

std::string CHTLJSCompiler::compileScriptLoader(const std::string& content) {
    // 编译脚本加载器
    std::string result = "// Script Loader\n";
    
    // 解析脚本加载器内容
    std::string loaderName = extractFunctionName(content);
    std::string properties = extractObjectProperties(content);
    
    result += "const " + loaderName + " = {\n";
    result += properties;
    result += "};\n";
    
    return result;
}

std::string CHTLJSCompiler::compileUtil(const std::string& content) {
    // 编译工具函数
    std::string result = "// Utility Functions\n";
    
    // 解析工具函数内容
    std::string functionName = extractFunctionName(content);
    std::string parameters = extractFunctionParameters(content);
    std::string body = extractFunctionBody(content);
    
    result += "function " + functionName + "(" + parameters + ") {\n";
    result += body;
    result += "}\n";
    
    // 注册到上下文
    if (context_) {
        context_->registerFunction(functionName, result);
    }
    
    return result;
}

void CHTLJSCompiler::setContext(std::shared_ptr<CHTLJSContext> context) {
    context_ = context;
}

void CHTLJSCompiler::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

std::string CHTLJSCompiler::compileExpression(const std::string& expression) {
    // 编译表达式
    std::string result = expression;
    
    // 处理增强选择器
    result = convertEnhancedSelector(result);
    
    // 处理增强监听器
    result = convertEnhancedListener(result);
    
    // 处理事件绑定
    result = convertEventBinding(result);
    
    // 处理事件委托
    result = convertEventDelegation(result);
    
    // 处理动画
    result = convertAnimation(result);
    
    // 处理虚对象
    result = convertVirtualObject(result);
    
    // 处理路由器
    result = convertRouter(result);
    
    return result;
}

std::string CHTLJSCompiler::convertEnhancedSelector(const std::string& selector) {
    // 转换增强选择器 {{CSS选择器}}
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::string result = std::regex_replace(selector, selectorRegex, "document.querySelector('$1')");
    
    return result;
}

std::string CHTLJSCompiler::convertEnhancedListener(const std::string& listener) {
    // 转换增强监听器
    std::string result = listener;
    
    // 处理事件绑定操作符 &->
    std::regex bindingRegex(R"((\w+)\s*&->\s*(\w+))");
    result = std::regex_replace(result, bindingRegex, "$1.addEventListener('$2', ");
    
    return result;
}

std::string CHTLJSCompiler::convertEventBinding(const std::string& binding) {
    // 转换事件绑定
    std::string result = binding;
    
    // 处理事件绑定操作符
    std::regex bindingRegex(R"((\w+)\s*&->\s*(\w+)\s*\(\s*([^)]*)\s*\))");
    result = std::regex_replace(result, bindingRegex, "$1.addEventListener('$2', function($3) {");
    
    return result;
}

std::string CHTLJSCompiler::convertEventDelegation(const std::string& delegation) {
    // 转换事件委托
    std::string result = delegation;
    
    // 处理事件委托语法
    std::regex delegationRegex(R"(Delegate\s*\{\s*parent:\s*([^,]+),\s*target:\s*([^,]+),\s*event:\s*([^,]+),\s*handler:\s*([^}]+)\s*\})");
    result = std::regex_replace(result, delegationRegex, 
        "document.querySelector('$1').addEventListener('$3', function(e) { if (e.target.matches('$2')) { $4(e); } });");
    
    return result;
}

std::string CHTLJSCompiler::convertAnimation(const std::string& animation) {
    // 转换动画
    std::string result = animation;
    
    // 处理动画语法
    std::regex animationRegex(R"(Animate\s*\{\s*name:\s*([^,]+),\s*properties:\s*([^}]+)\s*\})");
    result = std::regex_replace(result, animationRegex, 
        "const $1 = { properties: $2, start: function() { /* animation logic */ } };");
    
    return result;
}

std::string CHTLJSCompiler::convertVirtualObject(const std::string& virtualObject) {
    // 转换虚对象
    std::string result = virtualObject;
    
    // 处理虚对象语法
    std::regex virtualObjectRegex(R"(Vir\s*\{\s*name:\s*([^,]+),\s*properties:\s*([^}]+)\s*\})");
    result = std::regex_replace(result, virtualObjectRegex, 
        "const $1 = { properties: $2 };");
    
    return result;
}

std::string CHTLJSCompiler::convertRouter(const std::string& router) {
    // 转换路由器
    std::string result = router;
    
    // 处理路由器语法
    std::regex routerRegex(R"(Router\s*\{\s*name:\s*([^,]+),\s*routes:\s*([^}]+)\s*\})");
    result = std::regex_replace(result, routerRegex, 
        "const $1 = { routes: $2, navigate: function(url) { if (this.routes[url]) this.routes[url](); } };");
    
    return result;
}

std::string CHTLJSCompiler::processPlaceholders(const std::string& content) {
    // 处理占位符
    std::string result = content;
    
    // 处理占位符替换
    if (context_) {
        // 这里可以添加占位符处理逻辑
    }
    
    return result;
}

std::string CHTLJSCompiler::replacePlaceholders(const std::string& content, const std::unordered_map<std::string, std::string>& placeholders) {
    std::string result = content;
    
    for (const auto& placeholder : placeholders) {
        size_t pos = 0;
        while ((pos = result.find(placeholder.first, pos)) != std::string::npos) {
            result.replace(pos, placeholder.first.length(), placeholder.second);
            pos += placeholder.second.length();
        }
    }
    
    return result;
}

std::string CHTLJSCompiler::optimizeCode(const std::string& code) {
    // 代码优化
    std::string result = code;
    
    // 移除多余的空格
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 移除多余的分号
    std::regex semicolonRegex(R"(;\s*;)");
    result = std::regex_replace(result, semicolonRegex, ";");
    
    return result;
}

std::string CHTLJSCompiler::minifyCode(const std::string& code) {
    // 代码压缩
    std::string result = code;
    
    // 移除注释
    std::regex commentRegex(R"(//.*?\n)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空格
    result = optimizeCode(result);
    
    return result;
}

void CHTLJSCompiler::reportError(const std::string& message, size_t line, size_t column) {
    if (errorHandler_) {
        errorHandler_(message, line, column);
    } else {
        std::cerr << "CHTL JS Compiler Error at " << line << ":" << column << ": " << message << std::endl;
    }
}

std::string CHTLJSCompiler::extractBlockContent(const std::string& content, size_t start, size_t& end) {
    size_t pos = start;
    
    // 跳过关键字
    while (pos < content.length() && std::isalnum(content[pos])) {
        pos++;
    }
    
    // 跳过空格
    while (pos < content.length() && std::isspace(content[pos])) {
        pos++;
    }
    
    // 查找块开始
    if (pos < content.length() && content[pos] == '{') {
        pos++;
        
        int braceCount = 1;
        size_t blockStart = pos;
        
        while (pos < content.length() && braceCount > 0) {
            if (content[pos] == '{') braceCount++;
            else if (content[pos] == '}') braceCount--;
            pos++;
        }
        
        end = pos;
        return content.substr(blockStart, pos - blockStart - 1);
    }
    
    end = pos;
    return "";
}

std::string CHTLJSCompiler::extractFunctionName(const std::string& function) {
    std::regex nameRegex(R"(\w+)");
    std::smatch match;
    
    if (std::regex_search(function, match, nameRegex)) {
        return match.str();
    }
    
    return "";
}

std::string CHTLJSCompiler::extractFunctionParameters(const std::string& function) {
    std::regex paramRegex(R"(\w+\s*\(\s*([^)]*)\s*\))");
    std::smatch match;
    
    if (std::regex_search(function, match, paramRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string CHTLJSCompiler::extractFunctionBody(const std::string& function) {
    std::regex bodyRegex(R"(\w+\s*\([^)]*\)\s*\{\s*([^}]*)\s*\})");
    std::smatch match;
    
    if (std::regex_search(function, match, bodyRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string CHTLJSCompiler::extractObjectProperties(const std::string& object) {
    std::regex propRegex(R"(\w+\s*:\s*[^,}]+)");
    std::smatch match;
    std::string result;
    
    std::string::const_iterator start = object.begin();
    while (std::regex_search(start, object.end(), match, propRegex)) {
        if (!result.empty()) {
            result += ",\n  ";
        }
        result += match.str();
        start = match[0].second;
    }
    
    return result;
}

std::string CHTLJSCompiler::extractArrayElements(const std::string& array) {
    std::regex elemRegex(R"([^,]+)");
    std::smatch match;
    std::string result;
    
    std::string::const_iterator start = array.begin();
    while (std::regex_search(start, array.end(), match, elemRegex)) {
        if (!result.empty()) {
            result += ", ";
        }
        result += match.str();
        start = match[0].second;
    }
    
    return result;
}

bool CHTLJSCompiler::isValidCHTLJSSyntax(const std::string& content) {
    // 基本的语法验证
    return !content.empty();
}

bool CHTLJSCompiler::isValidFunction(const std::string& function) {
    // 函数语法验证
    std::regex functionRegex(R"(\w+\s*\([^)]*\)\s*\{[^}]*\})");
    return std::regex_match(function, functionRegex);
}

bool CHTLJSCompiler::isValidObject(const std::string& object) {
    // 对象语法验证
    std::regex objectRegex(R"(\{\s*[^}]*\})");
    return std::regex_match(object, objectRegex);
}

bool CHTLJSCompiler::isValidArray(const std::string& array) {
    // 数组语法验证
    std::regex arrayRegex(R"(\[\s*[^\]]*\])");
    return std::regex_match(array, arrayRegex);
}

bool CHTLJSCompiler::isValidExpression(const std::string& expression) {
    // 表达式语法验证
    return !expression.empty();
}

std::string CHTLJSCompiler::generateFunctionWrapper(const std::string& functionName, const std::string& parameters, const std::string& body) {
    std::ostringstream oss;
    oss << "function " << functionName << "(" << parameters << ") {\n";
    oss << "  " << body << "\n";
    oss << "}";
    return oss.str();
}

std::string CHTLJSCompiler::generateObjectWrapper(const std::string& objectName, const std::string& properties) {
    std::ostringstream oss;
    oss << "const " << objectName << " = {\n";
    oss << "  " << properties << "\n";
    oss << "};";
    return oss.str();
}

std::string CHTLJSCompiler::generateArrayWrapper(const std::string& arrayName, const std::string& elements) {
    std::ostringstream oss;
    oss << "const " << arrayName << " = [\n";
    oss << "  " << elements << "\n";
    oss << "];";
    return oss.str();
}

std::string CHTLJSCompiler::generateEventWrapper(const std::string& eventName, const std::string& handler) {
    std::ostringstream oss;
    oss << "document.addEventListener('" << eventName << "', " << handler << ");";
    return oss.str();
}

std::string CHTLJSCompiler::generateAnimationWrapper(const std::string& animationName, const std::string& properties) {
    std::ostringstream oss;
    oss << "const " << animationName << " = {\n";
    oss << "  " << properties << ",\n";
    oss << "  start: function() {\n";
    oss << "    // Animation logic\n";
    oss << "  }\n";
    oss << "};";
    return oss.str();
}

std::string CHTLJSCompiler::generateRouterWrapper(const std::string& routerName, const std::string& routes) {
    std::ostringstream oss;
    oss << "const " << routerName << " = {\n";
    oss << "  routes: {\n";
    oss << "    " << routes << "\n";
    oss << "  },\n";
    oss << "  navigate: function(url) {\n";
    oss << "    if (this.routes[url]) {\n";
    oss << "      this.routes[url]();\n";
    oss << "    }\n";
    oss << "  }\n";
    oss << "};";
    return oss.str();
}

} // namespace CHTL