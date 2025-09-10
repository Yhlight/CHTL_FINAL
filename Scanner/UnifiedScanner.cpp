#include "UnifiedScanner.h"
#include <iostream>

UnifiedScanner::UnifiedScanner(const std::string& source) : source(source), cursor(0) {}

std::vector<CodeBlock> UnifiedScanner::scan() {
    std::vector<CodeBlock> blocks;
    size_t last_pos = 0;

    while (cursor < source.length()) {
        // Find the next script or style block
        size_t script_pos = source.find("script {", cursor);
        size_t style_pos = source.find("style {", cursor);
        size_t next_block_pos = std::string::npos;

        if (script_pos != std::string::npos && style_pos != std::string::npos) {
            next_block_pos = std::min(script_pos, style_pos);
        } else if (script_pos != std::string::npos) {
            next_block_pos = script_pos;
        } else {
            next_block_pos = style_pos;
        }

        if (next_block_pos == std::string::npos) {
            // No more special blocks, the rest is CHTL
            if (cursor < source.length()) {
                blocks.push_back({CodeBlockType::CHTL, source.substr(cursor)});
            }
            break;
        }

        // Add the CHTL part before the block
        if (next_block_pos > cursor) {
            blocks.push_back({CodeBlockType::CHTL, source.substr(cursor, next_block_pos - cursor)});
        }

        // Find the end of the block
        size_t block_start = source.find('{', next_block_pos);
        size_t content_start = block_start + 1;
        int brace_level = 1;
        size_t content_end = content_start;

        for (size_t i = content_start; i < source.length(); ++i) {
            if (source[i] == '{') {
                brace_level++;
            } else if (source[i] == '}') {
                brace_level--;
                if (brace_level == 0) {
                    content_end = i;
                    break;
                }
            }
        }

        if (content_end > content_start) {
            std::string block_content = source.substr(content_start, content_end - content_start);
            if (source.substr(next_block_pos, 6) == "script") {
                 scanScriptContent(block_content, blocks);
            } else {
                 blocks.push_back({CodeBlockType::CSS, block_content});
            }
        }

        cursor = content_end + 1;
    }

    return blocks;
}

void UnifiedScanner::scanScriptContent(const std::string& content, std::vector<CodeBlock>& blocks) {
    // This is a highly simplified implementation of the placeholder mechanism.
    // A real implementation would need a more robust parser to distinguish
    // JS functions/objects from CHTL JS functions/objects.

    // For now, let's look for CHTL JS keywords.
    if (content.find("listen {") != std::string::npos ||
        content.find("animate {") != std::string::npos ||
        content.find("delegate {") != std::string::npos ||
        content.find("{{") != std::string::npos)
    {
        // Assume it's a mix and for now, treat the whole block as CHTL_JS
        blocks.push_back({CodeBlockType::CHTL_JS, content});
    } else {
        // Assume it's pure JS
        blocks.push_back({CodeBlockType::JS, content});
    }
}
