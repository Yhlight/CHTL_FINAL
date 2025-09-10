#pragma once

#include <string>
#include <vector>
#include <map>

enum class CodeBlockType {
    CHTL,
    CSS,
    JS,
    CHTL_JS
};

struct CodeBlock {
    CodeBlockType type;
    std::string content;
};

class UnifiedScanner {
public:
    UnifiedScanner(const std::string& source);
    std::vector<CodeBlock> scan();

private:
    std::string source;
    size_t cursor;
    std::map<std::string, std::string> js_placeholders;
    int placeholder_count = 0;

    // Helper methods for scanning different parts
    void scanScriptContent(const std::string& content, std::vector<CodeBlock>& blocks);
};
