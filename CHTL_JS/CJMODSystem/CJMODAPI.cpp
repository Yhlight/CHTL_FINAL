#include "CJMODAPI.h"
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& value : values) {
        args.emplace_back(AtomArg::Type::VALUE, value);
    }
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.type == AtomArg::Type::PLACEHOLDER && arg.value == placeholder) {
            arg.bind(func);
        }
    }
}

std::string Arg::match(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.type == AtomArg::Type::PLACEHOLDER && arg.value == placeholder) {
            arg.bind(func);
            return arg.getValue();
        }
    }
    return "";
}

void Arg::fillValue(const Arg& result) {
    if (result.args.size() != args.size()) {
        return; // 大小不匹配
    }
    
    for (size_t i = 0; i < args.size(); ++i) {
        args[i].fillValue(result.args[i].getValue());
    }
}

void Arg::transform(const std::string& template_str) {
    // 简单的模板替换实现
    std::string result = template_str;
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), args[i].getValue());
            pos += args[i].getValue().length();
        }
    }
    
    // 更新第一个参数的值
    if (!args.empty()) {
        args[0].fillValue(result);
    }
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << args[i].getValue() << "\"";
    }
    std::cout << "]" << std::endl;
}

Arg Syntax::analyze(const std::string& syntax) {
    Arg result;
    
    // 简单的语法分析，查找占位符
    std::regex placeholderRegex(R"(\$[?!_]*)");
    std::sregex_iterator iter(syntax.begin(), syntax.end(), placeholderRegex);
    std::sregex_iterator end;
    
    size_t lastPos = 0;
    for (; iter != end; ++iter) {
        // 添加占位符前的文本
        if (iter->position() > lastPos) {
            std::string text = syntax.substr(lastPos, iter->position() - lastPos);
            if (!text.empty()) {
                result.args.emplace_back(AtomArg::Type::VALUE, text);
            }
        }
        
        // 添加占位符
        std::string placeholder = iter->str();
        AtomArg::Type type = AtomArg::Type::PLACEHOLDER;
        
        if (placeholder == "$?") {
            type = AtomArg::Type::OPTIONAL_PLACEHOLDER;
        } else if (placeholder == "$!") {
            type = AtomArg::Type::REQUIRED_PLACEHOLDER;
        } else if (placeholder == "$_") {
            type = AtomArg::Type::UNORDERED_PLACEHOLDER;
        } else if (placeholder == "...") {
            type = AtomArg::Type::VARIADIC_PLACEHOLDER;
        }
        
        result.args.emplace_back(type, placeholder);
        lastPos = iter->position() + iter->length();
    }
    
    // 添加剩余的文本
    if (lastPos < syntax.length()) {
        std::string text = syntax.substr(lastPos);
        if (!text.empty()) {
            result.args.emplace_back(AtomArg::Type::VALUE, text);
        }
    }
    
    return result;
}

bool Syntax::isObject(const std::string& content) {
    std::regex objectRegex(R"(\{[^}]*\})");
    return std::regex_match(content, objectRegex);
}

bool Syntax::isFunction(const std::string& content) {
    std::regex functionRegex(R"(function\s+\w+\s*\([^)]*\)\s*\{[^}]*\})");
    return std::regex_match(content, functionRegex);
}

bool Syntax::isArray(const std::string& content) {
    std::regex arrayRegex(R"(\[[^\]]*\])");
    return std::regex_match(content, arrayRegex);
}

bool Syntax::isCHTLJSFunction(const std::string& content) {
    // 检测CHTL JS函数语法：functionName { key: value, ... }
    std::regex chtljsFunctionRegex(R"(\w+\s*\{[^}]*\})");
    return std::regex_match(content, chtljsFunctionRegex);
}

Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    Arg result;
    
    // 简单的扫描实现，查找关键字
    for (const auto& arg : args.args) {
        if (arg.type == AtomArg::Type::VALUE) {
            if (arg.value.find(keyword) != std::string::npos) {
                result.args.emplace_back(AtomArg::Type::VALUE, arg.value);
            }
        } else {
            result.args.push_back(arg);
        }
    }
    
    return result;
}

void CJMODGenerator::exportResult(const Arg& args) {
    // 简单的导出实现
    for (const auto& arg : args.args) {
        std::cout << arg.getValue();
    }
    std::cout << std::endl;
}

CHTLJSFunction CHTLJSFunction::CreateCHTLJSFunction(const std::string& syntax) {
    CHTLJSFunction func;
    func.syntax_ = syntax;
    func.supportsVirtualObject_ = true;
    
    // 解析参数
    Arg parsed = Syntax::analyze(syntax);
    for (const auto& arg : parsed.args) {
        if (arg.type == AtomArg::Type::PLACEHOLDER || 
            arg.type == AtomArg::Type::OPTIONAL_PLACEHOLDER ||
            arg.type == AtomArg::Type::REQUIRED_PLACEHOLDER ||
            arg.type == AtomArg::Type::UNORDERED_PLACEHOLDER) {
            func.parameters_.push_back(arg.value);
        }
    }
    
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    // 绑定虚对象支持
    // 这里可以实现虚对象的绑定逻辑
}

std::string CHTLJSFunction::generate(const std::vector<std::string>& args) {
    // 简单的代码生成
    std::string result = syntax_;
    
    for (size_t i = 0; i < parameters_.size() && i < args.size(); ++i) {
        size_t pos = 0;
        while ((pos = result.find(parameters_[i], pos)) != std::string::npos) {
            result.replace(pos, parameters_[i].length(), args[i]);
            pos += args[i].length();
        }
    }
    
    return result;
}

} // namespace CHTL