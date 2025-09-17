#include "CJMODScanner.h"
#include "Syntax.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace CJMOD {

// 静态成员初始化
bool CJMODScanner::caseSensitive = true;
bool CJMODScanner::ignoreWhitespace = false;
size_t CJMODScanner::maxScanLength = 10000;
size_t CJMODScanner::scanCount = 0;
size_t CJMODScanner::matchCount = 0;

Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    scanCount++;
    
    // 将Arg转换为字符串进行扫描
    std::string source = args.format();
    return scan(source, keyword);
}

Arg CJMODScanner::scan(const std::string& source, const std::string& keyword) {
    scanCount++;
    
    if (source.empty() || keyword.empty()) {
        return Arg();
    }
    
    // 预处理关键字
    std::string processedKeyword = preprocessKeyword(keyword);
    
    // 使用双指针扫描
    return dualPointerScan(source, processedKeyword);
}

std::string CJMODScanner::preExtract(const std::string& source, const std::string& keyword) {
    return extractBeforeKeyword(source, keyword);
}

std::string CJMODScanner::postExtract(const std::string& source, const std::string& keyword) {
    return extractAfterKeyword(source, keyword);
}

bool CJMODScanner::hasSyntaxFragment(const std::string& source, const std::string& keyword) {
    std::vector<std::string> fragments = extractSyntaxFragments(source, keyword);
    return !fragments.empty();
}

std::vector<std::string> CJMODScanner::extractSyntaxFragments(const std::string& source, const std::string& keyword) {
    std::vector<std::string> fragments;
    std::vector<std::pair<size_t, size_t>> boundaries = findSyntaxBoundaries(source, keyword);
    
    for (const auto& boundary : boundaries) {
        std::string fragment = extractBoundaryContent(source, boundary.first, boundary.second);
        if (!fragment.empty()) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<std::pair<size_t, size_t>> CJMODScanner::findSyntaxBoundaries(const std::string& source, const std::string& keyword) {
    std::vector<std::pair<size_t, size_t>> boundaries;
    
    if (source.empty() || keyword.empty()) {
        return boundaries;
    }
    
    size_t pos = 0;
    while (pos < source.length()) {
        size_t keywordPos = findNextKeyword(source, keyword, pos);
        if (keywordPos == std::string::npos) {
            break;
        }
        
        // 查找语法边界
        size_t start = keywordPos;
        size_t end = keywordPos + keyword.length();
        
        // 向前查找开始边界
        while (start > 0 && !isDelimiter(source[start - 1])) {
            start--;
        }
        
        // 向后查找结束边界
        while (end < source.length() && !isDelimiter(source[end])) {
            end++;
        }
        
        if (isValidBoundary(source, start, end)) {
            boundaries.emplace_back(start, end);
        }
        
        pos = end;
    }
    
    return boundaries;
}

bool CJMODScanner::isSyntaxBoundary(const std::string& source, size_t position, const std::string& keyword) {
    if (position >= source.length() || keyword.empty()) {
        return false;
    }
    
    // 检查关键字匹配
    if (!matchesKeyword(source, position, keyword)) {
        return false;
    }
    
    // 检查边界条件
    return isWordBoundary(source, position) && isWordBoundary(source, position + keyword.length());
}

Arg CJMODScanner::slidingWindowScan(const std::string& source, const std::string& keyword) {
    return slidingWindowScan(source, std::vector<std::string>{keyword});
}

Arg CJMODScanner::slidingWindowScan(const std::string& source, const std::vector<std::string>& keywords) {
    scanCount++;
    
    if (source.empty() || keywords.empty()) {
        return Arg();
    }
    
    Arg result;
    size_t windowStart = 0;
    size_t windowEnd = std::min(maxScanLength, source.length());
    
    while (windowStart < source.length()) {
        // 在当前窗口中查找关键字
        for (const auto& keyword : keywords) {
            Arg windowResult = windowScan(source, keyword, windowStart, windowEnd);
            if (!windowResult.empty()) {
                result.merge(windowResult);
                matchCount++;
            }
        }
        
        // 移动窗口
        windowStart = windowEnd;
        windowEnd = std::min(windowStart + maxScanLength, source.length());
    }
    
    return result;
}

std::string CJMODScanner::preprocessKeyword(const std::string& keyword) {
    std::string processed = keyword;
    
    if (!caseSensitive) {
        std::transform(processed.begin(), processed.end(), processed.begin(), ::tolower);
    }
    
    if (ignoreWhitespace) {
        processed = normalizeWhitespace(processed);
    }
    
    return processed;
}

std::vector<std::string> CJMODScanner::expandKeyword(const std::string& keyword) {
    std::vector<std::string> expanded;
    
    // 基本关键字
    expanded.push_back(keyword);
    
    // 添加变体
    if (caseSensitive) {
        std::string lower = keyword;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower != keyword) {
            expanded.push_back(lower);
        }
        
        std::string upper = keyword;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper != keyword) {
            expanded.push_back(upper);
        }
    }
    
    // 添加空格变体
    if (!ignoreWhitespace) {
        std::string withSpaces = keyword;
        for (size_t i = 1; i < withSpaces.length(); ++i) {
            if (withSpaces[i] != ' ' && withSpaces[i-1] != ' ') {
                std::string variant = withSpaces;
                variant.insert(i, " ");
                expanded.push_back(variant);
            }
        }
    }
    
    return expanded;
}

void CJMODScanner::setCaseSensitive(bool sensitive) {
    caseSensitive = sensitive;
}

void CJMODScanner::setIgnoreWhitespace(bool ignore) {
    ignoreWhitespace = ignore;
}

void CJMODScanner::setMaxScanLength(size_t maxLength) {
    maxScanLength = maxLength;
}

size_t CJMODScanner::getScanCount() {
    return scanCount;
}

size_t CJMODScanner::getMatchCount() {
    return matchCount;
}

void CJMODScanner::resetStatistics() {
    scanCount = 0;
    matchCount = 0;
}

Arg CJMODScanner::dualPointerScan(const std::string& source, const std::string& keyword) {
    Arg result;
    
    if (source.empty() || keyword.empty()) {
        return result;
    }
    
    size_t leftPtr = 0;
    size_t rightPtr = 0;
    
    while (rightPtr < source.length()) {
        // 预先扫描是否存在关键字
        size_t keywordPos = findNextKeyword(source, keyword, rightPtr);
        if (keywordPos == std::string::npos) {
            // 没有找到关键字，移动左指针
            leftPtr = rightPtr + 1;
            rightPtr = leftPtr;
            continue;
        }
        
        // 找到了关键字，移动右指针准备收集
        rightPtr = keywordPos + keyword.length();
        
        // 收集语法片段
        std::string fragment = source.substr(leftPtr, rightPtr - leftPtr);
        if (!fragment.empty()) {
            result.addToken(fragment);
            matchCount++;
        }
        
        // 移动左指针到关键字之后
        leftPtr = rightPtr;
    }
    
    return result;
}

std::pair<size_t, size_t> CJMODScanner::findKeywordBounds(const std::string& source, const std::string& keyword, size_t startPos) {
    size_t keywordPos = findNextKeyword(source, keyword, startPos);
    if (keywordPos == std::string::npos) {
        return {std::string::npos, std::string::npos};
    }
    
    size_t start = keywordPos;
    size_t end = keywordPos + keyword.length();
    
    // 扩展边界以包含完整的语法单元
    while (start > 0 && !isDelimiter(source[start - 1])) {
        start--;
    }
    
    while (end < source.length() && !isDelimiter(source[end])) {
        end++;
    }
    
    return {start, end};
}

std::string CJMODScanner::extractBeforeKeyword(const std::string& source, const std::string& keyword) {
    size_t keywordPos = findNextKeyword(source, keyword, 0);
    if (keywordPos == std::string::npos) {
        return source;
    }
    
    return source.substr(0, keywordPos);
}

std::string CJMODScanner::extractAfterKeyword(const std::string& source, const std::string& keyword) {
    size_t keywordPos = findNextKeyword(source, keyword, 0);
    if (keywordPos == std::string::npos) {
        return "";
    }
    
    return source.substr(keywordPos + keyword.length());
}

bool CJMODScanner::isValidBoundary(const std::string& source, size_t start, size_t end) {
    if (start >= end || end > source.length()) {
        return false;
    }
    
    std::string content = source.substr(start, end - start);
    
    // 检查是否包含关键字
    bool hasKeyword = false;
    for (size_t i = 0; i <= content.length() - 1; ++i) {
        if (matchesKeyword(content, i, "**") || 
            matchesKeyword(content, i, "++") || 
            matchesKeyword(content, i, "--") ||
            matchesKeyword(content, i, "&&") ||
            matchesKeyword(content, i, "||")) {
            hasKeyword = true;
            break;
        }
    }
    
    return hasKeyword;
}

std::string CJMODScanner::extractBoundaryContent(const std::string& source, size_t start, size_t end) {
    if (start >= end || end > source.length()) {
        return "";
    }
    
    return source.substr(start, end - start);
}

Arg CJMODScanner::windowScan(const std::string& source, const std::string& keyword, size_t windowStart, size_t windowEnd) {
    Arg result;
    
    if (windowStart >= windowEnd || windowEnd > source.length()) {
        return result;
    }
    
    std::string window = source.substr(windowStart, windowEnd - windowStart);
    size_t pos = 0;
    
    while (pos < window.length()) {
        size_t keywordPos = findNextKeyword(window, keyword, pos);
        if (keywordPos == std::string::npos) {
            break;
        }
        
        // 提取关键字周围的上下文
        size_t contextStart = (keywordPos > 10) ? keywordPos - 10 : 0;
        size_t contextEnd = std::min(keywordPos + keyword.length() + 10, window.length());
        
        std::string context = window.substr(contextStart, contextEnd - contextStart);
        if (!context.empty()) {
            result.addToken(context);
        }
        
        pos = keywordPos + keyword.length();
    }
    
    return result;
}

size_t CJMODScanner::findNextKeyword(const std::string& source, const std::string& keyword, size_t startPos) {
    if (startPos >= source.length() || keyword.empty()) {
        return std::string::npos;
    }
    
    std::string searchSource = source;
    std::string searchKeyword = keyword;
    
    if (!caseSensitive) {
        std::transform(searchSource.begin(), searchSource.end(), searchSource.begin(), ::tolower);
        std::transform(searchKeyword.begin(), searchKeyword.end(), searchKeyword.begin(), ::tolower);
    }
    
    size_t pos = searchSource.find(searchKeyword, startPos);
    
    // 验证是否是完整的单词边界
    while (pos != std::string::npos) {
        if (isWordBoundary(source, pos) && isWordBoundary(source, pos + keyword.length())) {
            return pos;
        }
        pos = searchSource.find(searchKeyword, pos + 1);
    }
    
    return std::string::npos;
}

size_t CJMODScanner::findPrevKeyword(const std::string& source, const std::string& keyword, size_t startPos) {
    if (startPos == 0 || keyword.empty()) {
        return std::string::npos;
    }
    
    std::string searchSource = source;
    std::string searchKeyword = keyword;
    
    if (!caseSensitive) {
        std::transform(searchSource.begin(), searchSource.end(), searchSource.begin(), ::tolower);
        std::transform(searchKeyword.begin(), searchKeyword.end(), searchKeyword.begin(), ::tolower);
    }
    
    size_t pos = startPos;
    while (pos > 0) {
        if (pos >= keyword.length()) {
            size_t checkPos = pos - keyword.length();
            if (searchSource.substr(checkPos, keyword.length()) == searchKeyword) {
                if (isWordBoundary(source, checkPos) && isWordBoundary(source, checkPos + keyword.length())) {
                    return checkPos;
                }
            }
        }
        pos--;
    }
    
    return std::string::npos;
}

bool CJMODScanner::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool CJMODScanner::isDelimiter(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}' || 
           c == '[' || c == ']' || c == ',' || c == ';' || 
           c == ':' || c == '.' || c == '?' || c == '=' ||
           c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '%' || c == '!' || c == '&' || c == '|' ||
           c == '<' || c == '>' || c == '^' || c == '~';
}

bool CJMODScanner::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '%' || c == '=' || c == '!' || c == '&' || 
           c == '|' || c == '<' || c == '>' || c == '^' || 
           c == '~' || c == '?';
}

bool CJMODScanner::isQuote(char c) {
    return c == '"' || c == '\'' || c == '`';
}

std::string CJMODScanner::normalizeWhitespace(const std::string& text) {
    std::string result;
    bool inWhitespace = false;
    
    for (char c : text) {
        if (isWhitespace(c)) {
            if (!inWhitespace) {
                result += ' ';
                inWhitespace = true;
            }
        } else {
            result += c;
            inWhitespace = false;
        }
    }
    
    return result;
}

std::string CJMODScanner::trimWhitespace(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = text.find_last_not_of(" \t\n\r");
    return text.substr(start, end - start + 1);
}

bool CJMODScanner::matchesKeyword(const std::string& text, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > text.length()) {
        return false;
    }
    
    std::string substr = text.substr(pos, keyword.length());
    
    if (!caseSensitive) {
        std::transform(substr.begin(), substr.end(), substr.begin(), ::tolower);
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
        return substr == lowerKeyword;
    }
    
    return substr == keyword;
}

bool CJMODScanner::isWordBoundary(const std::string& text, size_t pos) {
    if (pos >= text.length()) {
        return true;
    }
    
    char c = text[pos];
    return !std::isalnum(c) && c != '_' && c != '$';
}

} // namespace CJMOD
} // namespace CHTL