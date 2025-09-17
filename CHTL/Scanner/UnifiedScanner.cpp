#include "UnifiedScanner.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : m_placeholderCounter(0) {
}

UnifiedScanner::~UnifiedScanner() {
}

UnifiedScanner::ScanResult UnifiedScanner::scan(const std::string& code) {
    ScanResult result;
    m_placeholderCounter = 0;
    m_placeholderMap.clear();
    
    // 首先处理全局样式块和局部脚本块
    std::string processedCode = code;
    
    // 处理全局样式块中的CHTL语法
    processedCode = processGlobalStyleBlocks(processedCode);
    
    // 处理局部脚本块中的混合语法
    processedCode = processLocalScriptBlocks(processedCode);
    
    // 使用双指针扫描分离代码片段
    result.fragments = dualPointerScan(processedCode, 0, processedCode.length());
    
    // 生成占位符映射
    result.placeholderMap = m_placeholderMap;
    result.processedCode = processedCode;
    
    return result;
}

std::string UnifiedScanner::restorePlaceholders(const std::string& processedCode, 
                                               const std::map<std::string, std::string>& placeholderMap) {
    std::string result = processedCode;
    
    for (const auto& pair : placeholderMap) {
        const std::string& placeholder = pair.first;
        const std::string& original = pair.second;
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), original);
            pos += original.length();
        }
    }
    
    return result;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::dualPointerScan(const std::string& code, size_t start, size_t end) {
    std::vector<CodeFragment> fragments;
    size_t front = start;
    size_t back = start;
    
    while (front < end) {
        // 寻找语法边界
        bool foundBoundary = false;
        CodeType detectedType = CodeType::UNKNOWN;
        
        // 检查CHTL语法边界
        if (isCHTLSyntaxBoundary(code, front)) {
            detectedType = CodeType::CHTL;
            foundBoundary = true;
        }
        // 检查CHTL JS语法边界
        else if (isCHTLJSSyntaxBoundary(code, front)) {
            detectedType = CodeType::CHTL_JS;
            foundBoundary = true;
        }
        
        if (foundBoundary) {
            // 收集代码片段
            if (front > back) {
                std::string content = code.substr(back, front - back);
                CodeType type = identifyCodeType(content);
                if (type != CodeType::UNKNOWN) {
                    fragments.emplace_back(type, content, back, front);
                }
            }
            
            // 处理检测到的语法
            size_t fragmentEnd = front;
            if (detectedType == CodeType::CHTL) {
                fragmentEnd = findCHTLBlockEnd(code, front);
            } else if (detectedType == CodeType::CHTL_JS) {
                fragmentEnd = findCHTLJSBlockEnd(code, front);
            }
            
            if (fragmentEnd > front) {
                std::string content = code.substr(front, fragmentEnd - front);
                fragments.emplace_back(detectedType, content, front, fragmentEnd);
                front = fragmentEnd;
            } else {
                front++;
            }
            back = front;
        } else {
            front++;
        }
    }
    
    // 处理剩余的代码
    if (front > back) {
        std::string content = code.substr(back, front - back);
        CodeType type = identifyCodeType(content);
        if (type != CodeType::UNKNOWN) {
            fragments.emplace_back(type, content, back, front);
        }
    }
    
    return fragments;
}

std::string UnifiedScanner::preExtract(const std::string& code, const std::string& keyword, size_t pos) {
    // 前置截取算法实现
    // 用于处理类似 arg ** arg2 这样的语法片段
    size_t start = pos;
    while (start > 0 && !std::isspace(code[start - 1])) {
        start--;
    }
    
    size_t end = pos + keyword.length();
    while (end < code.length() && !std::isspace(code[end])) {
        end++;
    }
    
    return code.substr(start, end - start);
}

UnifiedScanner::CodeType UnifiedScanner::identifyCodeType(const std::string& code) {
    // 简单的代码类型识别
    if (code.find("{{") != std::string::npos || 
        code.find("Listen") != std::string::npos ||
        code.find("Animate") != std::string::npos) {
        return CodeType::CHTL_JS;
    }
    
    if (code.find("div") != std::string::npos ||
        code.find("span") != std::string::npos ||
        code.find("style") != std::string::npos) {
        return CodeType::CHTL;
    }
    
    if (code.find("function") != std::string::npos ||
        code.find("const") != std::string::npos ||
        code.find("let") != std::string::npos) {
        return CodeType::JS;
    }
    
    if (code.find("{") != std::string::npos && 
        code.find(":") != std::string::npos) {
        return CodeType::CSS;
    }
    
    return CodeType::UNKNOWN;
}

bool UnifiedScanner::isCHTLSyntaxBoundary(const std::string& code, size_t pos) {
    // 检查CHTL语法边界
    if (pos >= code.length()) return false;
    
    // 检查元素开始
    if (code[pos] == '<' || std::isalpha(code[pos])) {
        return true;
    }
    
    // 检查模板语法
    if (pos + 1 < code.length() && code.substr(pos, 2) == "[T") {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSSyntaxBoundary(const std::string& code, size_t pos) {
    // 检查CHTL JS语法边界
    if (pos >= code.length()) return false;
    
    // 检查增强选择器
    if (pos + 1 < code.length() && code.substr(pos, 2) == "{{") {
        return true;
    }
    
    // 检查CHTL JS函数
    std::vector<std::string> keywords = {"Listen", "Animate", "Router", "Vir"};
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= code.length() && 
            code.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    
    return false;
}

std::string UnifiedScanner::generatePlaceholder(CodeType type, size_t index) {
    std::stringstream ss;
    ss << "_";
    
    switch (type) {
        case CodeType::CHTL:
            ss << "CHTL";
            break;
        case CodeType::CHTL_JS:
            ss << "CHTL_JS";
            break;
        case CodeType::CSS:
            ss << "CSS";
            break;
        case CodeType::JS:
            ss << "JS";
            break;
        default:
            ss << "UNKNOWN";
            break;
    }
    
    ss << "_PLACEHOLDER_" << index << "_";
    return ss.str();
}

std::string UnifiedScanner::processLocalStyleBlock(const std::string& code) {
    // 处理局部样式块中的CHTL语法
    // 这里需要实现局部样式块中允许的CHTL语法处理
    return code;
}

std::string UnifiedScanner::processLocalScriptBlock(const std::string& code) {
    // 处理局部脚本块中的混合语法
    // 这里需要实现CHTL、CHTL JS、JS的混合处理
    return code;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::separateCHTLJSAndJS(const std::string& code) {
    // 分离CHTL JS和JS代码
    std::vector<CodeFragment> fragments;
    
    // 使用占位符机制分离代码
    std::string processedCode = code;
    size_t placeholderIndex = 0;
    
    // 这里需要实现复杂的占位符替换逻辑
    // 将JS代码替换为占位符，保留CHTL JS代码
    
    return fragments;
}

size_t UnifiedScanner::findCHTLBlockEnd(const std::string& code, size_t start) {
    // 简单的CHTL块结束查找
    size_t braceCount = 0;
    size_t pos = start;
    
    while (pos < code.length()) {
        if (code[pos] == '{') {
            braceCount++;
        } else if (code[pos] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return pos + 1;
            }
        }
        pos++;
    }
    
    return code.length();
}

size_t UnifiedScanner::findCHTLJSBlockEnd(const std::string& code, size_t start) {
    // 简单的CHTL JS块结束查找
    size_t braceCount = 0;
    size_t pos = start;
    
    while (pos < code.length()) {
        if (code[pos] == '{') {
            braceCount++;
        } else if (code[pos] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return pos + 1;
            }
        }
        pos++;
    }
    
    return code.length();
}

std::string UnifiedScanner::processGlobalStyleBlocks(const std::string& code) {
    // 处理全局样式块中的CHTL语法
    return code;
}

std::string UnifiedScanner::processLocalScriptBlocks(const std::string& code) {
    // 处理局部脚本块中的混合语法
    return code;
}

} // namespace CHTL