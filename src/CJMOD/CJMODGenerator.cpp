#include "CJMOD/CJMODGenerator.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CJMOD {

std::string CJMODGenerator::s_outputFormat = "javascript";
bool CJMODGenerator::s_debugMode = false;
std::string CJMODGenerator::s_codeStyle = "standard";

std::string CJMODGenerator::exportResult(const Arg& args) {
    debugOutput("开始导出结果，参数数量: " + std::to_string(args.size()));
    
    if (args.empty()) {
        return "";
    }
    
    // 检查是否有转换结果
    if (!args.getResult().empty()) {
        std::string result = args.getResult();
        debugOutput("使用转换结果: " + result);
        return formatCode(result);
    }
    
    // 默认生成方式：将参数连接成字符串
    std::string result;
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) {
            result += " ";
        }
        result += args[i].getValue();
    }
    
    debugOutput("生成结果: " + result);
    return formatCode(result);
}

std::string CJMODGenerator::exportResult(const Arg& args, const std::string& templateStr) {
    debugOutput("开始导出结果，使用模板: " + templateStr);
    
    if (args.empty()) {
        return templateStr;
    }
    
    // 使用模板生成代码
    std::string result = templateStr;
    
    // 替换模板中的占位符
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i].isPlaceholder()) {
            std::string placeholder = args[i].getValue();
            std::string value = args[i].execute();
            
            // 替换所有出现的占位符
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
    }
    
    debugOutput("模板生成结果: " + result);
    return formatCode(result);
}

void CJMODGenerator::setOutputFormat(const std::string& format) {
    s_outputFormat = format;
    debugOutput("设置输出格式: " + format);
}

void CJMODGenerator::setDebugMode(bool enabled) {
    s_debugMode = enabled;
    debugOutput("设置调试模式: " + (enabled ? "启用" : "禁用"));
}

void CJMODGenerator::setCodeStyle(const std::string& style) {
    s_codeStyle = style;
    debugOutput("设置代码风格: " + style);
}

std::string CJMODGenerator::formatCode(const std::string& code) {
    std::string result = code;
    
    // 根据输出格式进行格式化
    if (s_outputFormat == "javascript") {
        result = formatJavaScript(result);
    } else if (s_outputFormat == "html") {
        result = formatHTML(result);
    } else if (s_outputFormat == "css") {
        result = formatCSS(result);
    }
    
    // 应用代码风格
    result = applyCodeStyle(result);
    
    debugOutput("格式化完成，原始长度: " + std::to_string(code.length()) + 
                ", 格式化后长度: " + std::to_string(result.length()));
    
    return result;
}

std::string CJMODGenerator::applyCodeStyle(const std::string& code) {
    std::string result = code;
    
    if (s_codeStyle == "standard") {
        // 标准风格：4个空格缩进
        result = applyStandardStyle(result);
    } else if (s_codeStyle == "compact") {
        // 紧凑风格：最小化空白
        result = applyCompactStyle(result);
    } else if (s_codeStyle == "expanded") {
        // 展开风格：最大可读性
        result = applyExpandedStyle(result);
    }
    
    return result;
}

std::string CJMODGenerator::generateIndent(int level) {
    if (s_codeStyle == "compact") {
        return "";
    }
    
    int indentSize = (s_codeStyle == "expanded") ? 2 : 4;
    return std::string(level * indentSize, ' ');
}

void CJMODGenerator::debugOutput(const std::string& message) {
    if (s_debugMode) {
        std::cout << "[CJMODGenerator] " << message << std::endl;
    }
}

// 辅助格式化函数
std::string CJMODGenerator::formatJavaScript(const std::string& code) {
    std::string result = code;
    
    // 基本的 JavaScript 格式化
    // 在分号后添加换行
    size_t pos = 0;
    while ((pos = result.find(";", pos)) != std::string::npos) {
        if (pos + 1 < result.length() && result[pos + 1] != '\n') {
            result.insert(pos + 1, "\n");
            pos += 2;
        } else {
            pos++;
        }
    }
    
    // 在大括号后添加换行
    pos = 0;
    while ((pos = result.find("}", pos)) != std::string::npos) {
        if (pos + 1 < result.length() && result[pos + 1] != '\n') {
            result.insert(pos + 1, "\n");
            pos += 2;
        } else {
            pos++;
        }
    }
    
    return result;
}

std::string CJMODGenerator::formatHTML(const std::string& code) {
    std::string result = code;
    
    // 基本的 HTML 格式化
    // 在标签后添加换行
    size_t pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        if (pos + 1 < result.length() && result[pos + 1] != '\n') {
            result.insert(pos + 1, "\n");
            pos += 2;
        } else {
            pos++;
        }
    }
    
    return result;
}

std::string CJMODGenerator::formatCSS(const std::string& code) {
    std::string result = code;
    
    // 基本的 CSS 格式化
    // 在分号后添加换行
    size_t pos = 0;
    while ((pos = result.find(";", pos)) != std::string::npos) {
        if (pos + 1 < result.length() && result[pos + 1] != '\n') {
            result.insert(pos + 1, "\n");
            pos += 2;
        } else {
            pos++;
        }
    }
    
    return result;
}

std::string CJMODGenerator::applyStandardStyle(const std::string& code) {
    // 标准风格实现
    return code;
}

std::string CJMODGenerator::applyCompactStyle(const std::string& code) {
    std::string result = code;
    
    // 移除多余的空白
    result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
        return std::isspace(c) && c != ' ';
    }), result.end());
    
    // 压缩多个空格
    size_t pos = 0;
    while ((pos = result.find("  ", pos)) != std::string::npos) {
        result.replace(pos, 2, " ");
        pos += 1;
    }
    
    return result;
}

std::string CJMODGenerator::applyExpandedStyle(const std::string& code) {
    // 展开风格实现
    return code;
}

} // namespace CJMOD