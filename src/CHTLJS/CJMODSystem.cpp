#include "CHTLJS/CJMODSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// Syntax 实现
std::vector<Syntax::Arg> Syntax::analyze(const std::string& syntax) {
    std::vector<Arg> args;
    std::vector<std::string> tokens = tokenize(syntax);
    
    for (const auto& token : tokens) {
        Arg arg;
        arg.value = token;
        arg.is_placeholder = isPlaceholder(token);
        arg.is_optional = token.find("?") != std::string::npos;
        arg.is_required = token.find("!") != std::string::npos;
        arg.is_unordered = token.find("_") != std::string::npos;
        arg.type = extractPlaceholderType(token);
        args.push_back(arg);
    }
    
    return args;
}

bool Syntax::isObject(const std::string& code) {
    // 简单的对象检测
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '{' && trimmed.back() == '}';
}

bool Syntax::isFunction(const std::string& code) {
    // 简单的函数检测
    std::regex function_regex(R"(function\s+\w+\s*\([^)]*\)\s*\{)");
    return std::regex_search(code, function_regex);
}

bool Syntax::isArray(const std::string& code) {
    // 简单的数组检测
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '[' && trimmed.back() == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // CHTL JS函数检测：包含{ }且不是普通JS语法
    if (code.find('{') == std::string::npos || code.find('}') == std::string::npos) {
        return false;
    }
    
    // 检查是否包含CHTL JS特有的语法
    return code.find("->") != std::string::npos || 
           code.find("{{") != std::string::npos ||
           code.find("listen") != std::string::npos ||
           code.find("animate") != std::string::npos;
}

std::vector<std::string> Syntax::tokenize(const std::string& syntax) {
    std::vector<std::string> tokens;
    std::string current_token;
    
    for (char c : syntax) {
        if (std::isspace(c)) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

bool Syntax::isPlaceholder(const std::string& token) {
    return token.front() == '$';
}

std::string Syntax::extractPlaceholderType(const std::string& token) {
    if (token.front() != '$') {
        return "literal";
    }
    
    if (token.find("?") != std::string::npos) {
        return "optional";
    } else if (token.find("!") != std::string::npos) {
        return "required";
    } else if (token.find("_") != std::string::npos) {
        return "unordered";
    } else if (token == "...") {
        return "variadic";
    } else {
        return "placeholder";
    }
}

// Arg 实现
Arg::Arg() = default;

Arg::Arg(const std::vector<Syntax::Arg>& args) : args_(args) {
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    binders_[placeholder] = func;
}

std::string Arg::match(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args_) {
        if (arg.value == placeholder) {
            arg.bind(func);
            return func(arg.value);
        }
    }
    return "";
}

void Arg::fillValue(const std::vector<Syntax::Arg>& values) {
    for (size_t i = 0; i < std::min(args_.size(), values.size()); ++i) {
        args_[i].fillValue(values[i].value);
    }
}

void Arg::transform(const std::string& template_str) {
    // 简单的模板转换实现
    std::string result = template_str;
    
    for (size_t i = 0; i < args_.size(); ++i) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), args_[i].getValue());
            pos += args_[i].getValue().length();
        }
    }
    
    // 这里应该更新args_的内容，简化实现
}

Syntax::Arg& Arg::operator[](size_t index) {
    return args_[index];
}

const Syntax::Arg& Arg::operator[](size_t index) const {
    return args_[index];
}

size_t Arg::size() const {
    return args_.size();
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << args_[i].value << "\"";
    }
    std::cout << "]" << std::endl;
}

std::string Arg::toString() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << args_[i].value << "\"";
    }
    oss << "]";
    return oss.str();
}

// CJMODScanner 实现
Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    // 简化的扫描实现
    Arg result = args;
    
    for (auto& arg : result) {
        if (arg.value.find(keyword) != std::string::npos) {
            arg.fillValue(extractCodeFragment(arg.value, keyword));
        }
    }
    
    return result;
}

Arg CJMODScanner::scan(const Arg& args) {
    return args; // 简化实现
}

std::string CJMODScanner::extractCodeFragment(const std::string& code, const std::string& keyword) {
    size_t pos = code.find(keyword);
    if (pos != std::string::npos) {
        return code.substr(pos, keyword.length());
    }
    return "";
}

bool CJMODScanner::isKeywordMatch(const std::string& code, const std::string& keyword) {
    return code.find(keyword) != std::string::npos;
}

// CJMODGenerator 实现
std::string CJMODGenerator::exportResult(const Arg& args) {
    return generateJS(args);
}

std::string CJMODGenerator::generateJS(const Arg& args) {
    std::ostringstream oss;
    
    for (const auto& arg : args) {
        oss << arg.getValue() << " ";
    }
    
    return oss.str();
}

std::string CJMODGenerator::processTemplate(const std::string& template_str, const Arg& args) {
    std::string result = template_str;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), args[i].getValue());
            pos += args[i].getValue().length();
        }
    }
    
    return result;
}

std::string CJMODGenerator::escapeJSString(const std::string& str) {
    std::string result = str;
    
    // 转义特殊字符
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\n", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    return result;
}

// AtomArg 实现
AtomArg::AtomArg() : type_(Type::PLACEHOLDER) {
}

AtomArg::AtomArg(const std::string& value, Type type) : value_(value), type_(type) {
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    binder_ = func;
}

void AtomArg::fillValue(const std::string& value) {
    value_ = value;
}

void AtomArg::fillValue(int value) {
    value_ = std::to_string(value);
}

std::string AtomArg::getValue() const {
    return value_;
}

AtomArg::Type AtomArg::getType() const {
    return type_;
}

bool AtomArg::isPlaceholder() const {
    return type_ == Type::PLACEHOLDER;
}

bool AtomArg::isOptional() const {
    return type_ == Type::OPTIONAL;
}

bool AtomArg::isRequired() const {
    return type_ == Type::REQUIRED;
}

bool AtomArg::isUnordered() const {
    return type_ == Type::UNORDERED;
}

bool AtomArg::isVariadic() const {
    return type_ == Type::VARIADIC;
}

// CHTLJSFunction 实现
std::unordered_map<std::string, CHTLJSFunction> CHTLJSFunction::function_registry_;

CHTLJSFunction::CHTLJSFunction() : supports_virtual_object_(false) {
}

CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& syntax) 
    : name_(name), syntax_(syntax), supports_virtual_object_(false) {
}

CHTLJSFunction CHTLJSFunction::createCHTLJSFunction(const std::string& syntax) {
    CHTLJSFunction func;
    func.syntax_ = syntax;
    func.supports_virtual_object_ = true;
    
    // 解析语法，提取函数名和参数
    std::vector<std::string> tokens = Syntax::tokenize(syntax);
    if (!tokens.empty()) {
        func.name_ = tokens[0];
    }
    
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    auto it = function_registry_.find(functionName);
    if (it != function_registry_.end()) {
        it->second.supports_virtual_object_ = true;
    }
}

void CHTLJSFunction::setName(const std::string& name) {
    name_ = name;
}

std::string CHTLJSFunction::getName() const {
    return name_;
}

void CHTLJSFunction::setSyntax(const std::string& syntax) {
    syntax_ = syntax;
}

std::string CHTLJSFunction::getSyntax() const {
    return syntax_;
}

void CHTLJSFunction::addParameter(const std::string& name, const std::string& type) {
    parameters_.emplace_back(name, type);
}

std::vector<std::pair<std::string, std::string>> CHTLJSFunction::getParameters() const {
    return parameters_;
}

void CHTLJSFunction::setBody(const std::string& body) {
    body_ = body;
}

std::string CHTLJSFunction::getBody() const {
    return body_;
}

std::string CHTLJSFunction::generateJS() const {
    std::ostringstream oss;
    
    oss << "function " << name_ << "(";
    
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters_[i].first;
    }
    
    oss << ") {";
    
    if (!body_.empty()) {
        oss << "\n" << body_ << "\n";
    }
    
    oss << "}";
    
    return oss.str();
}

bool CHTLJSFunction::validate() const {
    return !name_.empty() && !syntax_.empty();
}

// CJMODSystem 实现
CJMODSystem::CJMODSystem() : debug_mode_(false) {
    // 设置默认配置
    configurations_["DEBUG_MODE"] = "false";
    configurations_["SCAN_ALGORITHM"] = "double_pointer";
}

CJMODSystem::~CJMODSystem() = default;

bool CJMODSystem::loadModule(const std::string& path) {
    // 简化的模块加载实现
    modules_[path] = path;
    
    if (debug_mode_) {
        std::cout << "[CJMODSystem] Loaded module: " << path << std::endl;
    }
    
    return true;
}

bool CJMODSystem::unloadModule(const std::string& name) {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        modules_.erase(it);
        return true;
    }
    return false;
}

bool CJMODSystem::hasModule(const std::string& name) const {
    return modules_.find(name) != modules_.end();
}

void CJMODSystem::registerFunction(const std::string& name, const CHTLJSFunction& func) {
    functions_[name] = func;
    
    if (debug_mode_) {
        std::cout << "[CJMODSystem] Registered function: " << name << std::endl;
    }
}

CHTLJSFunction* CJMODSystem::getFunction(const std::string& name) {
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CJMODSystem::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

void CJMODSystem::addSyntaxExtension(const std::string& keyword, const std::string& handler) {
    syntax_extensions_[keyword] = handler;
    
    if (debug_mode_) {
        std::cout << "[CJMODSystem] Added syntax extension: " << keyword << std::endl;
    }
}

std::string CJMODSystem::getSyntaxExtension(const std::string& keyword) const {
    auto it = syntax_extensions_.find(keyword);
    if (it != syntax_extensions_.end()) {
        return it->second;
    }
    return "";
}

bool CJMODSystem::hasSyntaxExtension(const std::string& keyword) const {
    return syntax_extensions_.find(keyword) != syntax_extensions_.end();
}

std::string CJMODSystem::generateJS(const std::string& chtl_js_code) {
    // 简化的JS生成实现
    std::string result = chtl_js_code;
    
    // 处理CHTL JS特有的语法
    // 这里应该实现完整的CHTL JS到JS的转换
    
    return result;
}

void CJMODSystem::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

std::string CJMODSystem::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return "";
}

void CJMODSystem::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
    setConfiguration("DEBUG_MODE", enabled ? "true" : "false");
}

bool CJMODSystem::isDebugMode() const {
    return debug_mode_;
}

std::vector<std::string> CJMODSystem::doublePointerScan(const std::string& code, const std::string& keyword) {
    std::vector<std::string> results;
    size_t start = 0;
    size_t end = 0;
    
    while (start < code.length()) {
        // 查找关键字
        size_t keyword_pos = code.find(keyword, start);
        if (keyword_pos == std::string::npos) {
            break;
        }
        
        // 查找匹配的结束位置
        end = keyword_pos + keyword.length();
        while (end < code.length() && code[end] != ';' && code[end] != '\n') {
            end++;
        }
        
        results.push_back(code.substr(keyword_pos, end - keyword_pos));
        start = end + 1;
    }
    
    return results;
}

std::string CJMODSystem::preemptiveCapture(const std::string& code, const std::string& keyword) {
    // 前置截取实现
    size_t keyword_pos = code.find(keyword);
    if (keyword_pos != std::string::npos) {
        return code.substr(0, keyword_pos);
    }
    return code;
}

} // namespace CHTL