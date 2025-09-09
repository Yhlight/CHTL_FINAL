#include "CHTL_JS/UtilThenExpression.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL_JS {

UtilThenExpression::UtilThenExpression(const std::string& expression)
    : m_expression(expression)
    , m_isSingleLine(false)
    , m_debugMode(false)
{
    parseExpression();
}

void UtilThenExpression::setChangeCallback(const std::string& callback) {
    m_changeCallback = callback;
    m_isSingleLine = true;
}

void UtilThenExpression::setThenCallback(const std::string& callback) {
    m_thenCallback = callback;
}

void UtilThenExpression::setChangeCallbacks(const std::vector<std::string>& callbacks) {
    m_changeCallbacks = callbacks;
    m_isSingleLine = false;
}

void UtilThenExpression::setThenCallbacks(const std::vector<std::string>& callbacks) {
    m_thenCallbacks = callbacks;
}

std::string UtilThenExpression::generateJavaScript() const {
    debugOutput("开始生成 JavaScript 代码");
    
    std::ostringstream oss;
    
    // 生成变量声明
    oss << generateVariableDeclaration();
    
    // 生成条件监听器
    oss << generateConditionListener();
    
    // 生成变化处理代码
    oss << generateChangeHandler();
    
    // 生成完成处理代码
    oss << generateThenHandler();
    
    std::string result = oss.str();
    debugOutput("生成的 JavaScript 代码长度: " + std::to_string(result.length()));
    
    return result;
}

bool UtilThenExpression::validate() const {
    if (m_expression.empty()) {
        return false;
    }
    
    // 检查是否包含必要的操作符
    if (m_expression.find(">") == std::string::npos && 
        m_expression.find("<") == std::string::npos &&
        m_expression.find("==") == std::string::npos &&
        m_expression.find("!=") == std::string::npos) {
        return false;
    }
    
    // 检查是否有回调函数
    if (m_changeCallback.empty() && m_changeCallbacks.empty()) {
        return false;
    }
    
    if (m_thenCallback.empty() && m_thenCallbacks.empty()) {
        return false;
    }
    
    return true;
}

void UtilThenExpression::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void UtilThenExpression::parseExpression() {
    // 解析表达式，提取变量和操作符
    debugOutput("解析表达式: " + m_expression);
    
    // 简单的表达式解析
    // 这里可以根据需要实现更复杂的解析逻辑
}

std::string UtilThenExpression::generateConditionListener() const {
    std::ostringstream oss;
    
    oss << "    // 条件监听器\n";
    oss << "    let previousValue = " << m_expression << ";\n";
    oss << "    \n";
    oss << "    function checkCondition() {\n";
    oss << "        const currentValue = " << m_expression << ";\n";
    oss << "        \n";
    oss << "        if (currentValue !== previousValue) {\n";
    oss << "            // 条件发生变化\n";
    oss << "            handleChange();\n";
    oss << "            \n";
    oss << "            if (currentValue) {\n";
    oss << "                // 条件为真，执行完成回调\n";
    oss << "                handleThen();\n";
    oss << "            }\n";
    oss << "        }\n";
    oss << "        \n";
    oss << "        previousValue = currentValue;\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    // 定期检查条件\n";
    oss << "    setInterval(checkCondition, 100);\n";
    oss << "    \n";
    
    return oss.str();
}

std::string UtilThenExpression::generateChangeHandler() const {
    std::ostringstream oss;
    
    oss << "    function handleChange() {\n";
    
    if (m_isSingleLine) {
        oss << "        " << m_changeCallback << "\n";
    } else {
        for (const auto& callback : m_changeCallbacks) {
            oss << "        " << callback << "\n";
        }
    }
    
    oss << "    }\n";
    oss << "    \n";
    
    return oss.str();
}

std::string UtilThenExpression::generateThenHandler() const {
    std::ostringstream oss;
    
    oss << "    function handleThen() {\n";
    
    if (m_isSingleLine) {
        oss << "        " << m_thenCallback << "\n";
    } else {
        for (const auto& callback : m_thenCallbacks) {
            oss << "        " << callback << "\n";
        }
    }
    
    oss << "    }\n";
    oss << "    \n";
    
    return oss.str();
}

std::string UtilThenExpression::generateVariableDeclaration() const {
    std::ostringstream oss;
    
    oss << "// util...then 表达式: " << m_expression << "\n";
    oss << "(function() {\n";
    oss << "    'use strict';\n";
    oss << "    \n";
    
    return oss.str();
}

void UtilThenExpression::debugOutput(const std::string& message) const {
    if (m_debugMode) {
        std::cout << "[UtilThenExpression] " << message << std::endl;
    }
}

// UtilThenParser 实现
std::vector<UtilThenExpression> UtilThenParser::parse(const std::string& code) {
    std::vector<UtilThenExpression> expressions;
    
    size_t pos = 0;
    while (pos < code.length()) {
        // 查找 util 关键字
        size_t utilPos = findKeyword(code, "util", pos);
        if (utilPos == std::string::npos) {
            break;
        }
        
        // 查找 -> 操作符
        size_t arrowPos = findKeyword(code, "->", utilPos);
        if (arrowPos == std::string::npos) {
            break;
        }
        
        // 查找 change 关键字
        size_t changePos = findKeyword(code, "change", arrowPos);
        if (changePos == std::string::npos) {
            break;
        }
        
        // 查找 then 关键字
        size_t thenPos = findKeyword(code, "then", changePos);
        if (thenPos == std::string::npos) {
            break;
        }
        
        // 提取表达式
        std::string expression = extractExpression(code.substr(utilPos, arrowPos - utilPos));
        
        // 提取 change 部分
        std::string changeCode = extractChange(code.substr(changePos, thenPos - changePos));
        
        // 提取 then 部分
        std::string thenCode = extractThen(code.substr(thenPos));
        
        // 创建表达式对象
        UtilThenExpression expr(expression);
        
        // 检查是否为单行语句
        if (changeCode.find('{') == std::string::npos) {
            expr.setChangeCallback(changeCode);
        } else {
            // 多行语句，需要进一步解析
            std::vector<std::string> changeCallbacks;
            // 这里需要实现更复杂的解析逻辑
            expr.setChangeCallbacks(changeCallbacks);
        }
        
        if (thenCode.find('{') == std::string::npos) {
            expr.setThenCallback(thenCode);
        } else {
            std::vector<std::string> thenCallbacks;
            // 这里需要实现更复杂的解析逻辑
            expr.setThenCallbacks(thenCallbacks);
        }
        
        expressions.push_back(expr);
        
        pos = thenPos + 4; // 跳过 "then"
    }
    
    return expressions;
}

bool UtilThenParser::isUtilThenExpression(const std::string& code) {
    return code.find("util") != std::string::npos &&
           code.find("->") != std::string::npos &&
           code.find("change") != std::string::npos &&
           code.find("then") != std::string::npos;
}

std::string UtilThenParser::extractExpression(const std::string& code) {
    std::string result = code;
    
    // 移除 "util" 关键字
    size_t utilPos = result.find("util");
    if (utilPos != std::string::npos) {
        result = result.substr(utilPos + 4);
    }
    
    // 移除空白字符
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    
    return result;
}

std::string UtilThenParser::extractChange(const std::string& code) {
    std::string result = code;
    
    // 移除 "change" 关键字
    size_t changePos = result.find("change");
    if (changePos != std::string::npos) {
        result = result.substr(changePos + 6);
    }
    
    // 移除 "->" 操作符
    size_t arrowPos = result.find("->");
    if (arrowPos != std::string::npos) {
        result = result.substr(arrowPos + 2);
    }
    
    // 移除空白字符
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    
    return result;
}

std::string UtilThenParser::extractThen(const std::string& code) {
    std::string result = code;
    
    // 移除 "then" 关键字
    size_t thenPos = result.find("then");
    if (thenPos != std::string::npos) {
        result = result.substr(thenPos + 4);
    }
    
    // 移除 "->" 操作符
    size_t arrowPos = result.find("->");
    if (arrowPos != std::string::npos) {
        result = result.substr(arrowPos + 2);
    }
    
    // 移除空白字符
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    
    return result;
}

size_t UtilThenParser::findKeyword(const std::string& code, const std::string& keyword, size_t startPos) {
    return code.find(keyword, startPos);
}

std::string UtilThenParser::extractCodeBlock(const std::string& code, size_t startPos, size_t endPos) {
    if (startPos >= code.length() || endPos > code.length() || startPos >= endPos) {
        return "";
    }
    
    return code.substr(startPos, endPos - startPos);
}

void UtilThenParser::skipWhitespace(const std::string& code, size_t& pos) {
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
}

size_t UtilThenParser::findMatchingBrace(const std::string& code, size_t startPos) {
    int braceCount = 0;
    for (size_t i = startPos; i < code.length(); i++) {
        if (code[i] == '{') {
            braceCount++;
        } else if (code[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

} // namespace CHTL_JS