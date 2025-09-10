#include "UnifiedScanner.h"
#include <iostream>
#include <vector>
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner(std::string source) : source_code_(std::move(source)) {}

std::string UnifiedScanner::generatePlaceholder(CodeBlockType type) {
    std::string p_type;
    switch (type) {
        case CodeBlockType::GLOBAL_CSS: p_type = "CSS"; break;
        case CodeBlockType::SCRIPT:     p_type = "SCRIPT"; break;
        default:                        p_type = "UNKNOWN"; break;
    }
    return "__CHTL_PLACEHOLDER_" + p_type + "_" + std::to_string(placeholder_id_++) + "__";
}

void UnifiedScanner::scan() {
    std::string current_identifier;
    std::stringstream processed_stream;
    int brace_depth = 0;
    size_t i = 0;

    while (i < source_code_.length()) {
        char c = source_code_[i];

        if (std::isspace(c)) {
            processed_stream << c;
            i++;
            continue;
        }

        if (std::isalpha(c)) { // Start of a potential identifier
            current_identifier.clear();
            while (i < source_code_.length() && (std::isalnum(source_code_[i]) || source_code_[i] == '_')) {
                current_identifier += source_code_[i];
                i++;
            }
            // We have an identifier, now look for a '{'
            size_t next_brace = source_code_.find('{', i);
            if (next_brace != std::string::npos) {
                std::string in_between = source_code_.substr(i, next_brace - i);
                if (in_between.find_first_not_of(" \t\n\r") == std::string::npos) {
                    // This is a block opening
                    bool is_style = (current_identifier == "style");
                    bool is_script = (current_identifier == "script");

                    if ((is_style && brace_depth == 0) || is_script) {
                        int block_brace_depth = 1;
                        size_t block_start_pos = next_brace + 1;
                        size_t block_end_pos = block_start_pos;

                        while(block_end_pos < source_code_.length() && block_brace_depth > 0) {
                            if (source_code_[block_end_pos] == '{') {
                                block_brace_depth++;
                            } else if (source_code_[block_end_pos] == '}') {
                                block_brace_depth--;
                            }
                            block_end_pos++;
                        }

                        if (block_brace_depth == 0) {
                            std::string content = source_code_.substr(block_start_pos, block_end_pos - block_start_pos - 1);
                            CodeBlockType type = is_style ? CodeBlockType::GLOBAL_CSS : CodeBlockType::SCRIPT;
                            std::string placeholder = generatePlaceholder(type);

                            code_blocks_.push_back({type, content, placeholder});
                            processed_stream << current_identifier << " { " << placeholder << " }";

                            i = block_end_pos;
                            current_identifier.clear();
                            continue;
                        }
                    }
                }
            }
             processed_stream << current_identifier;
             current_identifier.clear();
        }

        if (source_code_[i] == '{') {
            brace_depth++;
        } else if (source_code_[i] == '}') {
            brace_depth--;
        }

        processed_stream << source_code_[i];
        i++;
    }

    processed_source_ = processed_stream.str();
}

std::string UnifiedScanner::getProcessedSource() const {
    return processed_source_;
}

const std::vector<CodeBlock>& UnifiedScanner::getCodeBlocks() const {
    return code_blocks_;
}

} // namespace CHTL
