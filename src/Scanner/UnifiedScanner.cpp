#include "UnifiedScanner.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : placeholderCounter(0) {}

UnifiedScanner::~UnifiedScanner() {}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& sourceCode) {
    placeholderMap.clear();
    placeholderCounter = 0;
    
    return dualPointerScan(sourceCode);
}

std::string UnifiedScanner::restorePlaceholders(const std::string& code) {
    std::string result = code;
    
    for (const auto& pair : placeholderMap) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

const std::map<std::string, std::string>& UnifiedScanner::getPlaceholderMap() const {
    return placeholderMap;
}

std::string UnifiedScanner::generatePlaceholder(CodeType type) {
    std::string prefix;
    switch (type) {
        case CodeType::JS:
            prefix = "_JS_CODE_PLACEHOLDER_";
            break;
        case CodeType::CSS:
            prefix = "_CSS_CODE_PLACEHOLDER_";
            break;
        case CodeType::CHTL:
            prefix = "_CHTL_CODE_PLACEHOLDER_";
            break;
        case CodeType::CHTL_JS:
            prefix = "_CHTL_JS_CODE_PLACEHOLDER_";
            break;
        default:
            prefix = "_UNKNOWN_CODE_PLACEHOLDER_";
            break;
    }
    
    return prefix + std::to_string(placeholderCounter++);
}

CodeType UnifiedScanner::detectCodeType(const std::string& code) {
    // 检测CHTL语法特征
    if (code.find("text {") != std::string::npos ||
        code.find("style {") != std::string::npos ||
        code.find("script {") != std::string::npos ||
        code.find("[Template]") != std::string::npos ||
        code.find("[Custom]") != std::string::npos ||
        code.find("[Import]") != std::string::npos ||
        code.find("[Namespace]") != std::string::npos ||
        code.find("[Origin]") != std::string::npos ||
        code.find("[Configuration]") != std::string::npos) {
        return CodeType::CHTL;
    }
    
    // 检测CHTL JS语法特征
    if (code.find("{{") != std::string::npos ||
        code.find("listen {") != std::string::npos ||
        code.find("animate {") != std::string::npos ||
        code.find("fileloader {") != std::string::npos ||
        code.find("router {") != std::string::npos ||
        code.find("vir ") != std::string::npos ||
        code.find("iNeverAway {") != std::string::npos ||
        code.find("printMylove {") != std::string::npos) {
        return CodeType::CHTL_JS;
    }
    
    // 检测CSS语法特征
    if (code.find("{") != std::string::npos && 
        (code.find(":") != std::string::npos || code.find(";") != std::string::npos) &&
        code.find("function") == std::string::npos &&
        code.find("var ") == std::string::npos &&
        code.find("let ") == std::string::npos &&
        code.find("const ") == std::string::npos) {
        return CodeType::CSS;
    }
    
    // 默认为JS
    return CodeType::JS;
}

bool UnifiedScanner::isCHTLBoundary(const std::string& code, size_t pos) {
    if (pos >= code.length()) return false;
    
    // 检测CHTL关键字
    std::vector<std::string> keywords = {
        "text {", "style {", "script {", "[Template]", "[Custom]", 
        "[Import]", "[Namespace]", "[Origin]", "[Configuration]"
    };
    
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= code.length() &&
            code.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSBoundary(const std::string& code, size_t pos) {
    if (pos >= code.length()) return false;
    
    // 检测CHTL JS关键字
    std::vector<std::string> keywords = {
        "{{", "listen {", "animate {", "fileloader {", "router {",
        "vir ", "iNeverAway {", "printMylove {"
    };
    
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= code.length() &&
            code.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCSSBoundary(const std::string& code, size_t pos) {
    if (pos >= code.length()) return false;
    
    // CSS特征检测
    return code[pos] == '{' || code[pos] == '}' || code[pos] == ';' || code[pos] == ':';
}

bool UnifiedScanner::isJSBoundary(const std::string& code, size_t pos) {
    if (pos >= code.length()) return false;
    
    // JS特征检测
    std::vector<std::string> keywords = {
        "function", "var ", "let ", "const ", "if ", "for ", "while ",
        "return ", "class ", "import ", "export "
    };
    
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= code.length() &&
            code.substr(pos, keyword.length()) == keyword) {
            return true;
        }
    }
    
    return false;
}

std::vector<CodeFragment> UnifiedScanner::dualPointerScan(const std::string& sourceCode) {
    std::vector<CodeFragment> fragments;
    size_t front = 0, back = 0;
    size_t codeLength = sourceCode.length();
    
    while (front < codeLength) {
        // 预先扫描寻找关键字
        bool foundKeyword = false;
        CodeType detectedType = CodeType::UNKNOWN;
        
        // 检测CHTL语法
        if (isCHTLBoundary(sourceCode, front)) {
            detectedType = CodeType::CHTL;
            foundKeyword = true;
        }
        // 检测CHTL JS语法
        else if (isCHTLJSBoundary(sourceCode, front)) {
            detectedType = CodeType::CHTL_JS;
            foundKeyword = true;
        }
        // 检测CSS语法
        else if (isCSSBoundary(sourceCode, front)) {
            detectedType = CodeType::CSS;
            foundKeyword = true;
        }
        // 检测JS语法
        else if (isJSBoundary(sourceCode, front)) {
            detectedType = CodeType::JS;
            foundKeyword = true;
        }
        
        if (foundKeyword) {
            // 找到关键字，开始收集代码
            back = front;
            
            // 根据类型处理代码块
            if (detectedType == CodeType::CHTL) {
                CodeFragment fragment = wideJudge(sourceCode, front);
                fragments.push_back(fragment);
                front = fragment.endPos;
            }
            else if (detectedType == CodeType::CHTL_JS) {
                CodeFragment fragment = strictJudge(sourceCode, front);
                fragments.push_back(fragment);
                front = fragment.endPos;
            }
            else if (detectedType == CodeType::CSS) {
                // CSS处理逻辑
                size_t endPos = findBlockEnd(sourceCode, front, '{', '}');
                if (endPos != std::string::npos) {
                    std::string content = sourceCode.substr(front, endPos - front + 1);
                    CodeFragment fragment(CodeType::CSS, content, front, endPos);
                    fragments.push_back(fragment);
                    front = endPos + 1;
                } else {
                    front++;
                }
            }
            else if (detectedType == CodeType::JS) {
                // JS处理逻辑
                size_t endPos = findBlockEnd(sourceCode, front, '{', '}');
                if (endPos != std::string::npos) {
                    std::string content = sourceCode.substr(front, endPos - front + 1);
                    CodeFragment fragment(CodeType::JS, content, front, endPos);
                    fragments.push_back(fragment);
                    front = endPos + 1;
                } else {
                    front++;
                }
            }
        } else {
            front++;
        }
    }
    
    return fragments;
}

CodeFragment UnifiedScanner::wideJudge(const std::string& code, size_t startPos) {
    // 宽判处理 - 处理大块CHTL代码
    size_t endPos = findBlockEnd(code, startPos, '{', '}');
    if (endPos == std::string::npos) {
        endPos = code.length() - 1;
    }
    
    std::string content = code.substr(startPos, endPos - startPos + 1);
    CodeType type = detectCodeType(content);
    
    // 生成占位符
    std::string placeholder = generatePlaceholder(type);
    placeholderMap[placeholder] = content;
    
    return CodeFragment(type, placeholder, startPos, endPos);
}

CodeFragment UnifiedScanner::strictJudge(const std::string& code, size_t startPos) {
    // 严判处理 - 处理CHTL JS和JS混合代码
    size_t endPos = startPos;
    
    // 寻找代码块结束位置
    if (code[startPos] == '{') {
        endPos = findBlockEnd(code, startPos, '{', '}');
    } else {
        // 寻找语句结束位置
        while (endPos < code.length() && code[endPos] != ';' && code[endPos] != '\n') {
            endPos++;
        }
    }
    
    if (endPos == std::string::npos) {
        endPos = code.length() - 1;
    }
    
    std::string content = code.substr(startPos, endPos - startPos + 1);
    CodeType type = detectCodeType(content);
    
    // 生成占位符
    std::string placeholder = generatePlaceholder(type);
    placeholderMap[placeholder] = content;
    
    return CodeFragment(type, placeholder, startPos, endPos);
}

CodeFragment UnifiedScanner::processLocalStyleCHTL(const std::string& code, size_t startPos) {
    // 处理局部样式块中的CHTL语法
    // 这里需要特殊处理局部样式块中允许的CHTL语法
    return wideJudge(code, startPos);
}

CodeFragment UnifiedScanner::processLocalScriptCHTL(const std::string& code, size_t startPos) {
    // 处理局部脚本块中的CHTL语法
    // 这里需要特殊处理局部脚本块中允许的CHTL语法
    return strictJudge(code, startPos);
}

// 辅助函数：查找代码块结束位置
size_t UnifiedScanner::findBlockEnd(const std::string& code, size_t startPos, char openChar, char closeChar) {
    int depth = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = startPos; i < code.length(); i++) {
        char c = code[i];
        
        if (!inString) {
            if (c == openChar) {
                depth++;
            } else if (c == closeChar) {
                depth--;
                if (depth == 0) {
                    return i;
                }
            } else if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            }
        } else {
            if (c == stringChar && code[i-1] != '\\') {
                inString = false;
            }
        }
    }
    
    return std::string::npos;
}

} // namespace CHTL