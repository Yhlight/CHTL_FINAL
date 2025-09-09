#include "CJMOD/CHTLJSFunction.h"
#include "CJMOD/Syntax.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CJMOD {

// 静态成员变量
static std::map<std::string, CHTLJSFunction> s_registeredFunctions;

CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& syntax)
    : m_name(name)
    , m_syntax(syntax)
    , m_supportsVirtualObject(false)
    , m_supportsUnorderedKeyValue(false)
    , m_supportsOptionalKeyValue(false)
    , m_supportsUnquotedLiterals(false)
{
    parseSyntax();
    extractParameters();
    analyzeSyntaxFeatures();
}

CHTLJSFunction CHTLJSFunction::CreateCHTLJSFunction(const std::string& syntax) {
    // 从语法中提取函数名
    std::string functionName = extractFunctionName(syntax);
    
    CHTLJSFunction function(functionName, syntax);
    
    // 注册函数
    s_registeredFunctions[functionName] = function;
    
    return function;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    auto it = s_registeredFunctions.find(functionName);
    if (it != s_registeredFunctions.end()) {
        it->second.m_supportsVirtualObject = true;
    }
}

void CHTLJSFunction::addParameterBinding(const std::string& placeholder, 
                                       std::function<std::string(const std::string&)> handler) {
    m_parameterBindings[placeholder] = handler;
}

std::string CHTLJSFunction::processCall(const std::map<std::string, std::string>& parameters) const {
    if (!validateParameters(parameters)) {
        return "";
    }
    
    std::string result = generateFunctionDefinition();
    result += generateParameterProcessing(parameters);
    
    return result;
}

bool CHTLJSFunction::validateParameters(const std::map<std::string, std::string>& parameters) const {
    // 检查必需参数
    for (const auto& required : m_requiredParameters) {
        if (parameters.find(required) == parameters.end()) {
            return false;
        }
    }
    
    // 验证参数类型
    for (const auto& param : parameters) {
        if (!validateParameterType(param.first, param.second)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CHTLJSFunction::getRequiredParameters() const {
    return m_requiredParameters;
}

std::vector<std::string> CHTLJSFunction::getOptionalParameters() const {
    return m_optionalParameters;
}

std::string CHTLJSFunction::generateFunctionDefinition() const {
    std::ostringstream oss;
    
    oss << "function " << m_name << "(params) {\n";
    oss << "    // CHTL JS Function: " << m_name << "\n";
    oss << "    // Syntax: " << m_syntax << "\n";
    
    if (m_supportsVirtualObject) {
        oss << "    // Supports virtual objects\n";
    }
    
    if (m_supportsUnorderedKeyValue) {
        oss << "    // Supports unordered key-value pairs\n";
    }
    
    if (m_supportsOptionalKeyValue) {
        oss << "    // Supports optional key-value pairs\n";
    }
    
    if (m_supportsUnquotedLiterals) {
        oss << "    // Supports unquoted literals\n";
    }
    
    oss << "    \n";
    oss << "    // Parameter validation\n";
    oss << "    if (!params || typeof params !== 'object') {\n";
    oss << "        throw new Error('Invalid parameters for " << m_name << "');\n";
    oss << "    }\n";
    oss << "    \n";
    
    return oss.str();
}

std::string CHTLJSFunction::generateVirtualObjectSupport() const {
    if (!m_supportsVirtualObject) {
        return "";
    }
    
    std::ostringstream oss;
    
    oss << "    // Virtual object support\n";
    oss << "    const virtualObject = {\n";
    oss << "        " << m_name << ": function(params) {\n";
    oss << "            return " << m_name << "(params);\n";
    oss << "        }\n";
    oss << "    };\n";
    oss << "    \n";
    oss << "    return virtualObject;\n";
    
    return oss.str();
}

void CHTLJSFunction::parseSyntax() {
    // 解析语法字符串，提取函数名和参数信息
    // 这里需要根据具体的语法规则进行解析
}

void CHTLJSFunction::extractParameters() {
    // 从语法中提取参数信息
    std::vector<std::string> placeholders = Syntax::extractPlaceholders(m_syntax);
    
    for (const auto& placeholder : placeholders) {
        if (placeholder.find('!') != std::string::npos) {
            // 必需参数
            std::string paramName = placeholder;
            paramName.erase(std::remove(paramName.begin(), paramName.end(), '!'), paramName.end());
            paramName.erase(std::remove(paramName.begin(), paramName.end(), '$'), paramName.end());
            m_requiredParameters.push_back(paramName);
        } else if (placeholder.find('?') != std::string::npos) {
            // 可选参数
            std::string paramName = placeholder;
            paramName.erase(std::remove(paramName.begin(), paramName.end(), '?'), paramName.end());
            paramName.erase(std::remove(paramName.begin(), paramName.end(), '$'), paramName.end());
            m_optionalParameters.push_back(paramName);
        } else {
            // 默认参数
            std::string paramName = placeholder;
            paramName.erase(std::remove(paramName.begin(), paramName.end(), '$'), paramName.end());
            m_requiredParameters.push_back(paramName);
        }
    }
}

void CHTLJSFunction::analyzeSyntaxFeatures() {
    // 分析语法特征
    m_supportsVirtualObject = m_syntax.find("vir") != std::string::npos;
    m_supportsUnorderedKeyValue = m_syntax.find("$_") != std::string::npos;
    m_supportsOptionalKeyValue = m_syntax.find("$?") != std::string::npos;
    m_supportsUnquotedLiterals = m_syntax.find("unquoted") != std::string::npos;
}

std::string CHTLJSFunction::generateParameterProcessing(const std::map<std::string, std::string>& parameters) const {
    std::ostringstream oss;
    
    oss << "    // Process parameters\n";
    
    for (const auto& param : parameters) {
        oss << "    const " << param.first << " = params." << param.first;
        
        // 检查是否有默认值
        if (m_optionalParameters.end() != std::find(m_optionalParameters.begin(), m_optionalParameters.end(), param.first)) {
            oss << " || " << generateDefaultValue(param.first);
        }
        
        oss << ";\n";
    }
    
    oss << "    \n";
    oss << "    // Function implementation\n";
    oss << "    // TODO: Implement " << m_name << " logic\n";
    oss << "    \n";
    oss << "    return result;\n";
    oss << "}\n";
    
    return oss.str();
}

std::string CHTLJSFunction::generateDefaultValue(const std::string& parameter) const {
    // 根据参数类型生成默认值
    if (parameter.find("url") != std::string::npos) {
        return "\"\"";
    } else if (parameter.find("mode") != std::string::npos) {
        return "\"auto\"";
    } else if (parameter.find("width") != std::string::npos || parameter.find("height") != std::string::npos) {
        return "0";
    } else if (parameter.find("scale") != std::string::npos) {
        return "1";
    } else {
        return "null";
    }
}

bool CHTLJSFunction::validateParameterType(const std::string& parameter, const std::string& value) const {
    // 基本的参数类型验证
    if (parameter.find("url") != std::string::npos) {
        return !value.empty() && (value[0] == '"' || value[0] == '\'');
    } else if (parameter.find("mode") != std::string::npos) {
        return value == "auto" || value == "ASCII" || value == "Pixel";
    } else if (parameter.find("width") != std::string::npos || 
               parameter.find("height") != std::string::npos ||
               parameter.find("scale") != std::string::npos) {
        return std::all_of(value.begin(), value.end(), [](char c) {
            return std::isdigit(c) || c == '.' || c == '-';
        });
    }
    
    return true;
}

// 辅助函数实现
std::string CHTLJSFunction::extractFunctionName(const std::string& syntax) {
    size_t spacePos = syntax.find(' ');
    if (spacePos != std::string::npos) {
        return syntax.substr(0, spacePos);
    }
    return syntax;
}

} // namespace CJMOD