#pragma once

#include <string>
#include <vector>
#include <map>

struct ScannedContent {
    std::string chtl_content;
    std::map<std::string, std::string> css_blocks;
    std::map<std::string, std::string> script_blocks;
};

class UnifiedScanner {
public:
    UnifiedScanner(const std::string& source);
    ScannedContent scan();

private:
    void scanForBlocks();
    std::string extractBlock(size_t& pos);

    std::string source;
    size_t current = 0;
    int css_block_count = 0;
    int script_block_count = 0;
};
