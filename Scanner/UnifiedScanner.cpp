#include "UnifiedScanner.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

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
    
    // 首先进行宽判扫描 - 处理大块CHTL代码
    fragments = wideScan(sourceCode);
    
    // 对每个片段进行严判扫描 - 处理CHTL JS和JS混合代码
    for (auto& fragment : fragments) {
        if (fragment.type == CodeType::CHTL_JS || fragment.type == CodeType::JS) {
            std::vector<CodeFragment> subFragments = strictScan(fragment.content);
            if (!subFragments.empty()) {
                // 替换原片段
                fragment = subFragments[0];
                if (subFragments.size() > 1) {
                    fragments.insert(fragments.end(), subFragments.begin() + 1, subFragments.end());
                }
            }
        }
    }
    
    // 按位置排序
    std::sort(fragments.begin(), fragments.end(), 
        [](const CodeFragment& a, const CodeFragment& b) {
            return a.startPos < b.startPos;
        });
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::wideScan(const std::string& content) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    size_t len = content.length();
    
    while (pos < len) {
        // 跳过空白字符
        while (pos < len && isWhitespace(content[pos])) {
            pos++;
        }
        
        if (pos >= len) break;
        
        // 检测CHTL语法边界
        if (detectCHTLSyntaxBoundary(content, pos)) {
            size_t blockEnd = findBlockEnd(content, pos);
            if (blockEnd != std::string::npos) {
                CodeFragment fragment;
                fragment.type = CodeType::CHTL;
                fragment.content = content.substr(pos, blockEnd - pos + 1);
                fragment.startPos = pos;
                fragment.endPos = blockEnd + 1;
                fragment.isPlaceholder = false;
                fragments.push_back(fragment);
                pos = blockEnd + 1;
                continue;
            }
        }
        
        // 检测CHTL JS语法边界
        if (detectCHTLJSSyntaxBoundary(content, pos)) {
            size_t blockEnd = findBlockEnd(content, pos);
            if (blockEnd != std::string::npos) {
                CodeFragment fragment;
                fragment.type = CodeType::CHTL_JS;
                fragment.content = content.substr(pos, blockEnd - pos + 1);
                fragment.startPos = pos;
                fragment.endPos = blockEnd + 1;
                fragment.isPlaceholder = false;
                fragments.push_back(fragment);
                pos = blockEnd + 1;
                continue;
            }
        }
        
        // 检测CSS语法边界
        if (isCSSSyntax(content.substr(pos))) {
            size_t blockEnd = findBlockEnd(content, pos);
            if (blockEnd != std::string::npos) {
                CodeFragment fragment;
                fragment.type = CodeType::CSS;
                fragment.content = content.substr(pos, blockEnd - pos + 1);
                fragment.startPos = pos;
                fragment.endPos = blockEnd + 1;
                fragment.isPlaceholder = false;
                fragments.push_back(fragment);
                pos = blockEnd + 1;
                continue;
            }
        }
        
        // 默认作为JS处理
        size_t nextBoundary = pos + 1;
        while (nextBoundary < len && !detectCHTLSyntaxBoundary(content, nextBoundary) 
               && !detectCHTLJSSyntaxBoundary(content, nextBoundary)
               && !isCSSSyntax(content.substr(nextBoundary))) {
            nextBoundary++;
        }
        
        if (nextBoundary > pos) {
            CodeFragment fragment;
            fragment.type = CodeType::JS;
            fragment.content = content.substr(pos, nextBoundary - pos);
            fragment.startPos = pos;
            fragment.endPos = nextBoundary;
            fragment.isPlaceholder = false;
            fragments.push_back(fragment);
            pos = nextBoundary;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::strictScan(const std::string& content) {
    std::vector<CodeFragment> fragments;
    
    // 应用占位符机制分离JS和CHTL JS代码
    std::string processedContent = applyPlaceholderMechanism(content);
    
    // 在占位符处理后的内容中查找CHTL JS语法
    std::regex chtljsRegex(R"(\{\{[^}]+\}\}|vir\s+\w+\s*=|listen\s*\{|animate\s*\{|router\s*\{|scriptloader\s*\{|iNeverAway\s*\{|printMylove\s*\{|util\s+.*->\s*change|\$\w+\$)");
    std::sregex_iterator iter(processedContent.begin(), processedContent.end(), chtljsRegex);
    std::sregex_iterator end;
    
    size_t lastPos = 0;
    for (; iter != end; ++iter) {
        // 处理占位符之间的JS代码
        if (iter->position() > lastPos) {
            std::string jsContent = processedContent.substr(lastPos, iter->position() - lastPos);
            if (!trim(jsContent).empty()) {
                CodeFragment fragment;
                fragment.type = CodeType::JS;
                fragment.content = jsContent;
                fragment.startPos = lastPos;
                fragment.endPos = iter->position();
                fragment.isPlaceholder = false;
                fragments.push_back(fragment);
            }
        }
        
        // 处理CHTL JS代码
        CodeFragment fragment;
        fragment.type = CodeType::CHTL_JS;
        fragment.content = iter->str();
        fragment.startPos = iter->position();
        fragment.endPos = iter->position() + iter->length();
        fragment.isPlaceholder = false;
        fragments.push_back(fragment);
        
        lastPos = iter->position() + iter->length();
    }
    
    // 处理剩余的JS代码
    if (lastPos < processedContent.length()) {
        std::string jsContent = processedContent.substr(lastPos);
        if (!trim(jsContent).empty()) {
            CodeFragment fragment;
            fragment.type = CodeType::JS;
            fragment.content = jsContent;
            fragment.startPos = lastPos;
            fragment.endPos = processedContent.length();
            fragment.isPlaceholder = false;
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::string UnifiedScanner::applyPlaceholderMechanism(const std::string& content) {
    std::string result = content;
    
    // 查找JS函数、对象等语法结构
    std::vector<std::pair<size_t, size_t>> jsBlocks;
    
    // 查找函数定义
    std::regex functionRegex(R"(\bfunction\s+\w+\s*\([^)]*\)\s*\{)");
    std::sregex_iterator funcIter(result.begin(), result.end(), functionRegex);
    std::sregex_iterator funcEnd;
    
    for (; funcIter != funcEnd; ++funcIter) {
        size_t start = funcIter->position();
        size_t end = findBlockEnd(result, start);
        if (end != std::string::npos) {
            jsBlocks.push_back({start, end + 1});
        }
    }
    
    // 查找箭头函数
    std::regex arrowRegex(R"(\w+\s*=>\s*\{)");
    std::sregex_iterator arrowIter(result.begin(), result.end(), arrowRegex);
    std::sregex_iterator arrowEnd;
    
    for (; arrowIter != arrowEnd; ++arrowIter) {
        size_t start = arrowIter->position();
        size_t end = findBlockEnd(result, start);
        if (end != std::string::npos) {
            jsBlocks.push_back({start, end + 1});
        }
    }
    
    // 查找对象字面量
    std::regex objectRegex(R"(\{[^}]*\})");
    std::sregex_iterator objIter(result.begin(), result.end(), objectRegex);
    std::sregex_iterator objEnd;
    
    for (; objIter != objEnd; ++objIter) {
        size_t start = objIter->position();
        size_t end = start + objIter->length();
        jsBlocks.push_back({start, end});
    }
    
    // 按位置排序
    std::sort(jsBlocks.begin(), jsBlocks.end());
    
    // 从后往前替换，避免位置偏移
    for (auto it = jsBlocks.rbegin(); it != jsBlocks.rend(); ++it) {
        std::string placeholder = createPlaceholder("JS_CODE");
        placeholderMap_[placeholder] = result.substr(it->first, it->second - it->first);
        result.replace(it->first, it->second - it->first, placeholder);
    }
    
    return result;
}

bool UnifiedScanner::detectCHTLSyntaxBoundary(const std::string& content, size_t pos) {
    if (pos >= content.length()) return false;
    
    // 检测CHTL特有的语法模式
    std::vector<std::string> patterns = {
        R"(\[Template\])",
        R"(\[Custom\])",
        R"(\[Origin\])",
        R"(\[Import\])",
        R"(\[Namespace\])",
        R"(\[Configuration\])",
        R"(\[Info\])",
        R"(\[Export\])",
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
        if (std::regex_search(content.substr(pos), regex)) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::detectCHTLJSSyntaxBoundary(const std::string& content, size_t pos) {
    if (pos >= content.length()) return false;
    
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
        if (std::regex_search(content.substr(pos), regex)) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCHTLSyntax(const std::string& content) {
    return detectCHTLSyntaxBoundary(content, 0);
}

bool UnifiedScanner::isCHTLJSSyntax(const std::string& content) {
    return detectCHTLJSSyntaxBoundary(content, 0);
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
        fragment.isPlaceholder = false;
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
        fragment.isPlaceholder = false;
        fragments.push_back(fragment);
    }
    
    return fragments;
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

void UnifiedScanner::debugFragment(const CodeFragment& fragment) const {
    if (debugMode_) {
        std::cout << "Fragment: " << static_cast<int>(fragment.type) 
                  << " at " << fragment.startPos << "-" << fragment.endPos
                  << " content: " << fragment.content.substr(0, 50) << "..." << std::endl;
    }
}

} // namespace CHTL