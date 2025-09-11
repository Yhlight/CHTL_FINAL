#include "UnifiedScanner.h"

ScannedSource UnifiedScanner::scan(const std::string& source) {
    std::string processedSource = source;
    std::unordered_map<std::string, std::string> scripts;
    size_t scriptCount = 0;
    size_t pos = 0;

    while ((pos = processedSource.find("script", pos)) != std::string::npos) {
        // Found "script", now find the opening brace, skipping whitespace
        size_t brace_open_pos = processedSource.find('{', pos + 6);
        if (brace_open_pos == std::string::npos) {
            pos += 6;
            continue; // Not a script block
        }

        // Check if there is only whitespace between "script" and "{"
        bool isBlock = true;
        for(size_t i = pos + 6; i < brace_open_pos; ++i) {
            if (!isspace(processedSource[i])) {
                isBlock = false;
                break;
            }
        }

        if (!isBlock) {
            pos += 6;
            continue;
        }

        // Find matching closing brace
        int brace_level = 1;
        size_t brace_close_pos = brace_open_pos + 1;
        while (brace_level > 0 && brace_close_pos < processedSource.length()) {
            if (processedSource[brace_close_pos] == '{') {
                brace_level++;
            } else if (processedSource[brace_close_pos] == '}') {
                brace_level--;
            }
            brace_close_pos++;
        }

        if (brace_level == 0) {
            // Found a full script block
            std::string placeholder = "__CHTL_SCRIPT_PLACEHOLDER_" + std::to_string(scriptCount) + "__";

            // Extract content (from after '{' to before '}')
            std::string scriptContent = processedSource.substr(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2);
            scripts[placeholder] = scriptContent;

            // Replace the content of `script { ... }` with the placeholder
            processedSource.replace(brace_open_pos + 1, brace_close_pos - brace_open_pos - 2, placeholder);

            // Continue searching from after this script block
            pos = brace_close_pos;
            scriptCount++;
        } else {
            // No matching brace found, move on
            pos = brace_open_pos;
        }
    }

    return {processedSource, scripts};
}
