#include "CJMOD/CJMODScanner.h"
#include <iostream>
#include <algorithm>

namespace CJMOD {

int CJMODScanner::s_scanMode = 0;
bool CJMODScanner::s_debugMode = false;

Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    debugOutput("开始扫描，关键字: " + keyword);
    
    if (args.empty()) {
        return Arg();
    }
    
    // 将参数转换为字符串进行扫描
    std::string content;
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) {
            content += " ";
        }
        content += args[i].getValue();
    }
    
    debugOutput("扫描内容: " + content);
    
    // 预处理内容
    std::string processedContent = preprocessContent(content);
    debugOutput("预处理后内容: " + processedContent);
    
    // 执行双指针扫描
    std::vector<std::string> scanResult = doublePointerScan(processedContent, keyword);
    
    // 创建结果 Arg 对象
    Arg result;
    for (const auto& item : scanResult) {
        result.add(item);
    }
    
    debugOutput("扫描完成，结果数量: " + std::to_string(result.size()));
    return result;
}

Arg CJMODScanner::scan(const Arg& args) {
    // 默认扫描模式，不指定关键字
    return scan(args, "");
}

void CJMODScanner::setScanMode(int mode) {
    s_scanMode = mode;
    debugOutput("设置扫描模式: " + std::to_string(mode));
}

void CJMODScanner::setDebugMode(bool enabled) {
    s_debugMode = enabled;
    debugOutput("设置调试模式: " + (enabled ? "启用" : "禁用"));
}

std::vector<std::string> CJMODScanner::doublePointerScan(const std::string& content, const std::string& keyword) {
    std::vector<std::string> result;
    
    if (content.empty()) {
        return result;
    }
    
    size_t front = 0;
    size_t back = 0;
    
    debugOutput("开始双指针扫描，内容长度: " + std::to_string(content.length()));
    
    while (front < content.length()) {
        // 预先扫描是否存在关键字
        size_t keywordPos = findKeyword(content, keyword, front);
        
        if (keywordPos == std::string::npos) {
            // 没有找到关键字，移动前指针到末尾
            if (back < content.length()) {
                std::string remaining = content.substr(back);
                if (!remaining.empty()) {
                    result.push_back(remaining);
                }
            }
            break;
        }
        
        // 找到关键字，通知后指针准备收集
        if (back < keywordPos) {
            std::string beforeKeyword = content.substr(back, keywordPos - back);
            if (!beforeKeyword.empty()) {
                result.push_back(beforeKeyword);
            }
        }
        
        // 添加关键字
        result.push_back(keyword);
        
        // 更新指针位置
        front = keywordPos + keyword.length();
        back = front;
        
        debugOutput("找到关键字位置: " + std::to_string(keywordPos) + ", 前指针: " + std::to_string(front));
    }
    
    debugOutput("双指针扫描完成，结果数量: " + std::to_string(result.size()));
    return result;
}

std::string CJMODScanner::preprocessContent(const std::string& content) {
    std::string result = content;
    
    // 移除多余的空白字符
    result.erase(std::remove_if(result.begin(), result.end(), [](char c) {
        return std::isspace(c) && c != ' ';
    }), result.end());
    
    // 压缩多个空格为单个空格
    size_t pos = 0;
    while ((pos = result.find("  ", pos)) != std::string::npos) {
        result.replace(pos, 2, " ");
        pos += 1;
    }
    
    // 去除首尾空白
    if (!result.empty() && result[0] == ' ') {
        result = result.substr(1);
    }
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    debugOutput("预处理完成，原始长度: " + std::to_string(content.length()) + 
                ", 处理后长度: " + std::to_string(result.length()));
    
    return result;
}

bool CJMODScanner::isKeyword(const std::string& content, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > content.length()) {
        return false;
    }
    
    return content.substr(pos, keyword.length()) == keyword;
}

size_t CJMODScanner::findKeyword(const std::string& content, const std::string& keyword, size_t startPos) {
    if (keyword.empty()) {
        return std::string::npos;
    }
    
    return content.find(keyword, startPos);
}

void CJMODScanner::debugOutput(const std::string& message) {
    if (s_debugMode) {
        std::cout << "[CJMODScanner] " << message << std::endl;
    }
}

} // namespace CJMOD