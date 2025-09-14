#include "UnifiedScanner.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

UnifiedScanner::UnifiedScanner() 
    : placeholderPrefix_("_JS_CODE_PLACEHOLDER_")
    , placeholderCounter_(0)
    , debugMode_(false) {
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& sourceCode) {
    if (debugMode_) {
        std::cout << "Starting unified scan of " << sourceCode.length() << " characters" << std::endl;
    }
    
    return performScan(sourceCode);
}

std::vector<CodeFragment> UnifiedScanner::performScan(const std::string& sourceCode) {
    std::vector<CodeFragment> fragments;
    
    // 首先进行双指针扫描
    fragments = dualPointerScan(sourceCode);
    
    // 处理局部样式块和脚本块
    std::vector<CodeFragment> localStyles = extractLocalStyleBlocks(sourceCode);
    std::vector<CodeFragment> localScripts = extractLocalScriptBlocks(sourceCode);
    
    // 合并结果
    fragments.insert(fragments.end(), localStyles.begin(), localStyles.end());
    fragments.insert(fragments.end(), localScripts.begin(), localScripts.end());
    
    // 按位置排序
    std::sort(fragments.begin(), fragments.end(), 
        [](const CodeFragment& a, const CodeFragment& b) {
            return a.startPos < b.startPos;
        });
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::dualPointerScan(const std::string& content) {
    std::vector<CodeFragment> fragments;
    size_t front = 0, back = 0;
    size_t len = content.length();
    
    while (front < len) {
        // 寻找语法边界
        if (detectSyntaxBoundary(content, front)) {
            // 找到边界，准备收集代码片段
            if (front > back) {
                // 收集前面的代码
                std::string code = content.substr(back, front - back);
                if (!trim(code).empty()) {
                    CodeFragment fragment;
                    fragment.type = determineCodeType(code);
                    fragment.content = code;
                    fragment.startPos = back;
                    fragment.endPos = front;
                    fragments.push_back(fragment);
                }
            }
            
            // 处理当前语法块
            size_t blockEnd = findBlockEnd(content, front);
            if (blockEnd != std::string::npos) {
                std::string block = content.substr(front, blockEnd - front + 1);
                CodeFragment fragment;
                fragment.type = determineCodeType(block);
                fragment.content = block;
                fragment.startPos = front;
                fragment.endPos = blockEnd + 1;
                fragments.push_back(fragment);
                
                front = blockEnd + 1;
                back = front;
            } else {
                front++;
            }
        } else {
            front++;
        }
    }
    
    // 处理剩余代码
    if (front > back) {
        std::string code = content.substr(back, front - back);
        if (!trim(code).empty()) {
            CodeFragment fragment;
            fragment.type = determineCodeType(code);
            fragment.content = code;
            fragment.startPos = back;
            fragment.endPos = front;
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::extractLocalStyleBlocks(const std::string& content) {
    std::vector<CodeFragment> fragments;
    std::regex styleRegex(R"(style\s*\{[^}]*\})");
    std::sregex_iterator iter(content.begin(), content.end(), styleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        CodeFragment fragment;
        fragment.type = CodeType::CHTL;
        fragment.content = iter->str();
        fragment.startPos = iter->position();
        fragment.endPos = iter->position() + iter->length();
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::extractLocalScriptBlocks(const std::string& content) {
    std::vector<CodeFragment> fragments;
    std::regex scriptRegex(R"(script\s*\{[^}]*\})");
    std::sregex_iterator iter(content.begin(), content.end(), scriptRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        CodeFragment fragment;
        fragment.type = CodeType::CHTL_JS;
        fragment.content = iter->str();
        fragment.startPos = iter->position();
        fragment.endPos = iter->position() + iter->length();
        fragments.push_back(fragment);
    }
    
    return fragments;
}

bool UnifiedScanner::detectSyntaxBoundary(const std::string& content, size_t pos) {
    if (pos >= content.length()) return false;
    
    // 检测CHTL语法边界
    if (isCHTLSyntax(content.substr(pos))) {
        return true;
    }
    
    // 检测CHTL JS语法边界
    if (isCHTLJSSyntax(content.substr(pos))) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLSyntax(const std::string& content) {
    // 检测CHTL特有的语法模式
    std::vector<std::string> patterns = {
        R"(\[Template\])",
        R"(\[Custom\])",
        R"(\[Origin\])",
        R"(\[Import\])",
        R"(\[Namespace\])",
        R"(\[Configuration\])",
        R"(@Style)",
        R"(@Element)",
        R"(@Var)",
        R"(@Html)",
        R"(@JavaScript)",
        R"(@Chtl)",
        R"(@CJmod)",
        R"(text\s*\{)",
        R"(style\s*\{)",
        R"(script\s*\{)"
    };
    
    for (const auto& pattern : patterns) {
        std::regex regex(pattern);
        if (std::regex_search(content, regex)) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSSyntax(const std::string& content) {
    // 检测CHTL JS特有的语法模式
    std::vector<std::string> patterns = {
        R"(\{\{[^}]+\}\})",  // 增强选择器 {{selector}}
        R"(vir\s+\w+\s*=)",  // 虚对象
        R"(listen\s*\{)",    // 增强监听器
        R"(animate\s*\{)",   // 动画
        R"(router\s*\{)",    // 路由
        R"(scriptloader\s*\{)", // 脚本加载器
        R"(iNeverAway\s*\{)", // iNeverAway函数
        R"(printMylove\s*\{)", // printMylove函数
        R"(util\s+.*->\s*change)", // util...then表达式
        R"(\$\w+\$)"         // 响应式值
    };
    
    for (const auto& pattern : patterns) {
        std::regex regex(pattern);
        if (std::regex_search(content, regex)) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCSSSyntax(const std::string& content) {
    // 检测CSS语法模式
    std::vector<std::string> patterns = {
        R"(\.[\w-]+\s*\{)",
        R"(#[\w-]+\s*\{)",
        R"([\w-]+\s*\{)",
        R"(@media)",
        R"(@keyframes)",
        R"(@import)",
        R"(@font-face)"
    };
    
    for (const auto& pattern : patterns) {
        std::regex regex(pattern);
        if (std::regex_search(content, regex)) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isJSSyntax(const std::string& content) {
    // 检测JavaScript语法模式
    std::vector<std::string> patterns = {
        R"(function\s+\w+\s*\()",
        R"(const\s+\w+\s*=)",
        R"(let\s+\w+\s*=)",
        R"(var\s+\w+\s*=)",
        R"(class\s+\w+)",
        R"(if\s*\()",
        R"(for\s*\()",
        R"(while\s*\()",
        R"(switch\s*\()",
        R"(try\s*\{)",
        R"(catch\s*\()",
        R"(throw\s+)",
        R"(return\s+)",
        R"(console\.log)",
        R"(document\.)",
        R"(window\.)"
    };
    
    for (const auto& pattern : patterns) {
        std::regex regex(pattern);
        if (std::regex_search(content, regex)) {
            return true;
        }
    }
    
    return false;
}

CodeType UnifiedScanner::determineCodeType(const std::string& content) {
    if (isCHTLSyntax(content)) {
        return CodeType::CHTL;
    } else if (isCHTLJSSyntax(content)) {
        return CodeType::CHTL_JS;
    } else if (isCSSSyntax(content)) {
        return CodeType::CSS;
    } else if (isJSSyntax(content)) {
        return CodeType::JS;
    }
    
    // 默认返回JS类型
    return CodeType::JS;
}

std::string UnifiedScanner::createPlaceholder(const std::string& type) {
    std::string placeholder = placeholderPrefix_ + "_" + type + "_" + std::to_string(placeholderCounter_++);
    return placeholder;
}

std::string UnifiedScanner::decodePlaceholder(const std::string& content) {
    std::string result = content;
    for (const auto& pair : placeholderMap_) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    return result;
}

std::string UnifiedScanner::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool UnifiedScanner::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool UnifiedScanner::isCommentStart(const std::string& content, size_t pos) {
    if (pos + 1 >= content.length()) return false;
    return content[pos] == '/' && content[pos + 1] == '/';
}

bool UnifiedScanner::isCommentEnd(const std::string& content, size_t pos, bool multiLine) {
    if (multiLine) {
        return pos + 1 < content.length() && content[pos] == '*' && content[pos + 1] == '/';
    } else {
        return content[pos] == '\n';
    }
}

bool UnifiedScanner::isStringDelimiter(char c) {
    return c == '"' || c == '\'';
}

bool UnifiedScanner::isBlockStart(const std::string& content, size_t pos) {
    return content[pos] == '{';
}

bool UnifiedScanner::isBlockEnd(const std::string& content, size_t pos) {
    return content[pos] == '}';
}

size_t UnifiedScanner::findBlockEnd(const std::string& content, size_t start) {
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    bool inComment = false;
    
    for (size_t i = start; i < content.length(); ++i) {
        char c = content[i];
        
        if (inComment) {
            if (c == '\n') {
                inComment = false;
            }
            continue;
        }
        
        if (c == '/' && i + 1 < content.length() && content[i + 1] == '/') {
            inComment = true;
            continue;
        }
        
        if (inString) {
            if (c == stringChar) {
                inString = false;
            }
            continue;
        }
        
        if (c == '"' || c == '\'') {
            inString = true;
            stringChar = c;
            continue;
        }
        
        if (c == '{') {
            braceCount++;
        } else if (c == '}') {
            braceCount--;
            if (braceCount == 0) {
                return i;
            }
        }
    }
    
    return std::string::npos;
}

} // namespace CHTL