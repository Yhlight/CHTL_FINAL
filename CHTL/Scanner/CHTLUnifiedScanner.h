#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include "CodeFragment.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const std::string& source);
    std::vector<CodeFragment> scan();
    const std::map<std::string, std::string>& getPlaceholders() const;


private:
    std::string source;
    size_t current = 0;
    int line = 1;
    std::vector<CodeFragment> fragments;
    std::map<std::string, std::string> placeholders;
    int placeholder_counter = 0;

    void scanSource();
    void scanStyleBlock();
    void scanScriptBlock();

    char advance();
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;
    void skipWhitespace();

    bool match(const std::string& expected);
    std::string generatePlaceholder(const std::string& content);

    // CHTL JS detection
    bool isChtlJsKeyword();
    bool isChtlJsEnhanceSelector();
    void scanChtlJs();
    void scanJs();

};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
