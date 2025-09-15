#include "CJMODAPI.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTLJS {

// AtomArg implementation
AtomArg::AtomArg(ArgType type, const std::string& value)
    : type_(type), value_(value) {
}

AtomArg::~AtomArg() {
}

AtomArg::ArgType AtomArg::getType() const {
    return type_;
}

const std::string& AtomArg::getValue() const {
    return value_;
}

void AtomArg::setValue(const std::string& value) {
    value_ = value;
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    binder_ = func;
}

void AtomArg::fillValue(const std::string& value) {
    value_ = value;
    if (binder_) {
        value_ = binder_(value);
    }
}

void AtomArg::fillValue(int value) {
    fillValue(std::to_string(value));
}

std::string AtomArg::toString() const {
    std::stringstream ss;
    ss << "AtomArg(" << static_cast<int>(type_) << ", \"" << value_ << "\")";
    return ss.str();
}

// Arg implementation
Arg::Arg() {
}

Arg::Arg(const std::vector<std::string>& values) {
    for (const std::string& value : values) {
        add(value);
    }
}

Arg::~Arg() {
}

void Arg::add(const std::string& value) {
    args_.push_back(std::make_shared<AtomArg>(AtomArg::ArgType::UNKNOWN, value));
}

void Arg::add(AtomArg* arg) {
    args_.push_back(std::shared_ptr<AtomArg>(arg));
}

size_t Arg::size() const {
    return args_.size();
}

AtomArg& Arg::operator[](size_t index) {
    return *args_[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return *args_[index];
}

void Arg::bind(const std::string& key, std::function<std::string(const std::string&)> func) {
    binders_[key] = func;
}

std::string Arg::match(const std::string& key, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args_) {
        if (arg->getValue() == key) {
            return func(arg->getValue());
        }
    }
    return "";
}

void Arg::fillValue(const Arg& other) {
    for (size_t i = 0; i < std::min(args_.size(), other.args_.size()); i++) {
        args_[i]->fillValue(other.args_[i]->getValue());
    }
}

void Arg::transform(const std::string& template_str) {
    std::string result = template_str;
    
    for (size_t i = 0; i < args_.size(); i++) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), args_[i]->getValue());
        }
    }
    
    // 更新所有参数的值
    for (size_t i = 0; i < args_.size(); i++) {
        args_[i]->setValue(result);
    }
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < args_.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << args_[i]->getValue() << "\"";
    }
    std::cout << "]" << std::endl;
}

std::string Arg::toString() const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < args_.size(); i++) {
        if (i > 0) ss << ", ";
        ss << "\"" << args_[i]->getValue() << "\"";
    }
    ss << "]";
    return ss.str();
}

// Syntax implementation
Arg Syntax::analyze(const std::string& syntax) {
    Arg args;
    std::vector<std::string> tokens = tokenize(syntax);
    
    for (const std::string& token : tokens) {
        if (token == "$") {
            args.add(new AtomArg(AtomArg::ArgType::PLACEHOLDER));
        } else if (token == "$?") {
            args.add(new AtomArg(AtomArg::ArgType::OPTIONAL_PLACEHOLDER));
        } else if (token == "$!") {
            args.add(new AtomArg(AtomArg::ArgType::REQUIRED_PLACEHOLDER));
        } else if (token == "$_") {
            args.add(new AtomArg(AtomArg::ArgType::UNORDERED_PLACEHOLDER));
        } else if (token == "...") {
            args.add(new AtomArg(AtomArg::ArgType::VARIADIC_PLACEHOLDER));
        } else {
            args.add(new AtomArg(AtomArg::ArgType::UNKNOWN, token));
        }
    }
    
    return args;
}

bool Syntax::isObject(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.length() >= 2 && 
           trimmed[0] == '{' && 
           trimmed[trimmed.length() - 1] == '}' &&
           isBalanced(trimmed);
}

bool Syntax::isFunction(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.find("function") == 0 || 
           trimmed.find("=>") != std::string::npos ||
           (trimmed.find("(") != std::string::npos && trimmed.find(")") != std::string::npos);
}

bool Syntax::isArray(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.length() >= 2 && 
           trimmed[0] == '[' && 
           trimmed[trimmed.length() - 1] == ']' &&
           isBalanced(trimmed);
}

bool Syntax::isCHTLJSFunction(const std::string& str) {
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    // 检查是否包含CHTL JS函数语法
    return trimmed.find("{") != std::string::npos && 
           trimmed.find("}") != std::string::npos &&
           (trimmed.find(":") != std::string::npos || trimmed.find("=") != std::string::npos);
}

std::vector<std::string> Syntax::tokenize(const std::string& syntax) {
    std::vector<std::string> tokens;
    std::string current;
    
    for (size_t i = 0; i < syntax.length(); i++) {
        char c = syntax[i];
        
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == '$') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            
            // 检查特殊占位符
            if (i + 1 < syntax.length()) {
                char next = syntax[i + 1];
                if (next == '?' || next == '!' || next == '_') {
                    tokens.push_back(std::string(1, c) + next);
                    i++; // 跳过下一个字符
                } else {
                    tokens.push_back(std::string(1, c));
                }
            } else {
                tokens.push_back(std::string(1, c));
            }
        } else if (c == '.' && i + 2 < syntax.length() && 
                   syntax[i + 1] == '.' && syntax[i + 2] == '.') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back("...");
            i += 2; // 跳过后面的两个点
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool Syntax::isBalanced(const std::string& str) {
    int brace_count = 0;
    int paren_count = 0;
    int bracket_count = 0;
    
    for (char c : str) {
        switch (c) {
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '(': paren_count++; break;
            case ')': paren_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
    }
    
    return brace_count == 0 && paren_count == 0 && bracket_count == 0;
}

// CJMODScanner implementation
Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    // 这里实现扫描逻辑
    // 暂时返回原始参数
    return args;
}

Arg CJMODScanner::scan(const std::string& source, const std::string& keyword) {
    Arg result;
    
    std::vector<std::string> positions = findKeywordPositions(source, keyword);
    for (const std::string& context : positions) {
        result.add(context);
    }
    
    return result;
}

std::vector<std::string> CJMODScanner::findKeywordPositions(const std::string& source, const std::string& keyword) {
    std::vector<std::string> positions;
    size_t pos = 0;
    
    while ((pos = source.find(keyword, pos)) != std::string::npos) {
        std::string context = extractContext(source, pos, keyword.length());
        positions.push_back(context);
        pos += keyword.length();
    }
    
    return positions;
}

std::string CJMODScanner::extractContext(const std::string& source, size_t pos, size_t length) {
    size_t start = pos;
    size_t end = pos + length;
    
    // 扩展上下文
    while (start > 0 && source[start - 1] != ' ' && source[start - 1] != '\n') {
        start--;
    }
    
    while (end < source.length() && source[end] != ' ' && source[end] != '\n') {
        end++;
    }
    
    return source.substr(start, end - start);
}

// CJMODGenerator implementation
void CJMODGenerator::exportResult(const Arg& args) {
    std::string code = generateCode(args);
    std::cout << code << std::endl;
}

std::string CJMODGenerator::generateCode(const Arg& args) {
    std::stringstream ss;
    
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) ss << " ";
        ss << args[i].getValue();
    }
    
    return ss.str();
}

std::string CJMODGenerator::processTemplate(const std::string& template_str, const Arg& args) {
    std::string result = template_str;
    
    for (size_t i = 0; i < args.size(); i++) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), args[i].getValue());
        }
    }
    
    return result;
}

// CHTLJSFunction implementation
std::shared_ptr<CHTLJSFunction> CHTLJSFunction::CreateCHTLJSFunction(const std::string& syntax) {
    // 解析语法字符串，提取函数名
    size_t space_pos = syntax.find(' ');
    if (space_pos == std::string::npos) {
        space_pos = syntax.find('{');
    }
    
    std::string functionName = syntax.substr(0, space_pos);
    auto func = std::make_shared<CHTLJSFunction>(functionName);
    func->syntax_template_ = syntax;
    
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    // 绑定虚对象支持
    // 这里可以实现虚对象的绑定逻辑
}

CHTLJSFunction::CHTLJSFunction(const std::string& functionName)
    : function_name_(functionName) {
}

std::string CHTLJSFunction::generateCode() const {
    std::stringstream ss;
    ss << function_name_ << "({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": \"" << param.second << "\"";
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

void CHTLJSFunction::addParameter(const std::string& key, const std::string& value) {
    string_parameters_[key] = value;
}

void CHTLJSFunction::addParameter(const std::string& key, std::shared_ptr<CHTLJSBaseNode> value) {
    node_parameters_[key] = value;
}

const std::string& CHTLJSFunction::getFunctionName() const {
    return function_name_;
}

const std::map<std::string, std::string>& CHTLJSFunction::getStringParameters() const {
    return string_parameters_;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& CHTLJSFunction::getNodeParameters() const {
    return node_parameters_;
}

} // namespace CHTLJS