#include "CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace CHTL {

std::optional<std::string> CHTLLoader::loadFile(const std::string& relative_path, const std::string& base_path) {
    try {
        std::filesystem::path full_path;
        if (!base_path.empty()) {
            full_path = std::filesystem::path(base_path).parent_path() / relative_path;
        } else {
            full_path = relative_path;
        }

        // Get the canonical path to handle relative paths and symlinks consistently.
        std::string canonical_path = std::filesystem::canonical(full_path).string();

        // Check if the file has already been included.
        auto it = std::find(included_files_.begin(), included_files_.end(), canonical_path);
        if (it != included_files_.end()) {
            // Already processed, return nullopt to signal the parser to skip.
            return std::nullopt;
        }

        std::ifstream file(canonical_path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << relative_path << std::endl;
            return std::nullopt;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        // Mark this file as included.
        included_files_.push_back(canonical_path);

        return buffer.str();

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem Error: " << e.what() << std::endl;
        return std::nullopt;
    }
}


void CHTLLoader::loadModule(const std::string& module_name, std::shared_ptr<ParserContext> context) {
    // Prevent circular module dependencies
    if (std::find(loaded_modules_.begin(), loaded_modules_.end(), module_name) != loaded_modules_.end()) {
        return;
    }
    loaded_modules_.push_back(module_name);

    // 1. Find the module directory
    // For now, we only check a local 'modules' directory
    std::filesystem::path module_dir = "modules";
    module_dir /= module_name;

    if (!std::filesystem::exists(module_dir)) {
        throw std::runtime_error("Module not found: " + module_name);
    }

    // 2. Parse the info file first (though we don't use the export map yet)
    std::filesystem::path info_file = module_dir / "info" / (module_name + ".chtl");
    if (auto info_content = loadFile(info_file.string(), "")) {
        CHTLLexer info_lexer(*info_content);
        auto info_tokens = info_lexer.scanTokens();
        // Use a temporary context for parsing the info file to not pollute the main one
        CHTLParser info_parser(*info_content, info_tokens, *this, info_file.string(), std::make_shared<ParserContext>());
        info_parser.parse(); // This will parse [Info] and [Export] blocks
    }

    // 3. Parse all source files in the module's src directory
    std::filesystem::path src_dir = module_dir / "src";
    if (std::filesystem::exists(src_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(src_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                if (auto src_content = loadFile(entry.path().string(), "")) {
                    CHTLLexer src_lexer(*src_content);
                    auto src_tokens = src_lexer.scanTokens();
                    // IMPORTANT: Parse the source files into the *main* context
                    CHTLParser src_parser(*src_content, src_tokens, *this, entry.path().string(), context);
                    src_parser.parse();
                }
            }
        }
    }
}


} // namespace CHTL
