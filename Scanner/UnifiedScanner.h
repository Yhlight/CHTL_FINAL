#pragma once

#include <string>
#include <vector>
#include <map>

class UnifiedScanner {
public:
    // The main method to scan the source code.
    // It will identify and separate different language blocks.
    std::string scan(const std::string& source);

    // Method to get the original code snippet from a placeholder.
    std::string getOriginalCode(const std::string& placeholder) const;

    // Method to get all the stored mappings.
    const std::map<std::string, std::string>& getPlaceholderMap() const;

private:
    // A map to store the relationship between a placeholder and the original code.
    std::map<std::string, std::string> placeholderMap;

    // A counter to ensure unique placeholders.
    int placeholderCounter = 0;

    // Generates a new, unique placeholder string.
    std::string generatePlaceholder(const std::string& code, const std::string& type);

    // Helper function to find the matching closing brace for a block.
    size_t findMatchingBrace(const std::string& str, size_t start_pos);

    // Processes the content of a style block to separate CHTL syntax.
    std::string processStyleBlock(const std::string& blockContent);

    // Processes the content of a script block to separate CHTL JS and JS.
    std::string processScriptBlock(const std::string& blockContent);

    // Recursive helper for script block processing.
    std::string processScriptBlock_recursive(const std::string& blockContent);

    // Placeholders
    static const std::string CHTL_PLACEHOLDER;
    static const std::string CHTLJS_PLACEHOLDER;
    static const std::string JS_PLACEHOLDER;
};
