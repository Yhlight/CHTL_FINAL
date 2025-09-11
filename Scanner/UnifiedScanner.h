#pragma once

#include <string>
#include <vector>
#include <map>

struct ScannedContent {
    std::string chtl_content;
    std::map<std::string, std::string> css_placeholders;
    std::map<std::string, std::string> js_placeholders;
};

class UnifiedScanner {
public:
    UnifiedScanner(const std::string& source);
    ScannedContent scan();

private:
    std::string generatePlaceholder(const std::string& type);

    std::string source;
    size_t current = 0;
    int css_block_count = 0;
    int js_block_count = 0;

    ScannedContent content;
};
