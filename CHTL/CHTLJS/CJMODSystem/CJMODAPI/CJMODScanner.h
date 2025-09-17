#ifndef CJMOD_SCANNER_H
#define CJMOD_SCANNER_H

#include "Arg.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace CJMOD {

class CJMODScanner {
public:
    // 双指针扫描
    static Arg scan(const Arg& args, const std::string& keyword);
    static Arg scan(const std::string& source, const std::string& keyword);
    
    // 前置截取
    static std::string preExtract(const std::string& source, const std::string& keyword);
    static std::string postExtract(const std::string& source, const std::string& keyword);
    
    // 语法片段识别
    static bool hasSyntaxFragment(const std::string& source, const std::string& keyword);
    static std::vector<std::string> extractSyntaxFragments(const std::string& source, const std::string& keyword);
    
    // 语法边界识别
    static std::vector<std::pair<size_t, size_t>> findSyntaxBoundaries(const std::string& source, const std::string& keyword);
    static bool isSyntaxBoundary(const std::string& source, size_t position, const std::string& keyword);
    
    // 滑动窗口扫描
    static Arg slidingWindowScan(const std::string& source, const std::string& keyword);
    static Arg slidingWindowScan(const std::string& source, const std::vector<std::string>& keywords);
    
    // 关键字预处理
    static std::string preprocessKeyword(const std::string& keyword);
    static std::vector<std::string> expandKeyword(const std::string& keyword);
    
    // 扫描配置
    static void setCaseSensitive(bool sensitive);
    static void setIgnoreWhitespace(bool ignore);
    static void setMaxScanLength(size_t maxLength);
    
    // 扫描统计
    static size_t getScanCount();
    static size_t getMatchCount();
    static void resetStatistics();
    
private:
    // 双指针扫描实现
    static Arg dualPointerScan(const std::string& source, const std::string& keyword);
    static std::pair<size_t, size_t> findKeywordBounds(const std::string& source, const std::string& keyword, size_t startPos);
    
    // 前置截取实现
    static std::string extractBeforeKeyword(const std::string& source, const std::string& keyword);
    static std::string extractAfterKeyword(const std::string& source, const std::string& keyword);
    
    // 语法边界实现
    static bool isValidBoundary(const std::string& source, size_t start, size_t end);
    static std::string extractBoundaryContent(const std::string& source, size_t start, size_t end);
    
    // 滑动窗口实现
    static Arg windowScan(const std::string& source, const std::string& keyword, size_t windowStart, size_t windowEnd);
    static size_t findNextKeyword(const std::string& source, const std::string& keyword, size_t startPos);
    static size_t findPrevKeyword(const std::string& source, const std::string& keyword, size_t startPos);
    
    // 辅助方法
    static bool isWhitespace(char c);
    static bool isDelimiter(char c);
    static bool isOperator(char c);
    static bool isQuote(char c);
    
    static std::string normalizeWhitespace(const std::string& text);
    static std::string trimWhitespace(const std::string& text);
    
    static bool matchesKeyword(const std::string& text, size_t pos, const std::string& keyword);
    static bool isWordBoundary(const std::string& text, size_t pos);
    
    // 配置选项
    static bool caseSensitive;
    static bool ignoreWhitespace;
    static size_t maxScanLength;
    
    // 统计信息
    static size_t scanCount;
    static size_t matchCount;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_SCANNER_H