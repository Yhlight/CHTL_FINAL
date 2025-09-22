#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "../CHTL/Config/Configuration.h"

namespace CHTL {

    enum class FragmentType {
        CHTL,
        CHTL_JS,
        CSS,
        JS,
        HTML, // Added for [Origin] @Html
        DIRECTIVE, // Added for 'use' statements
        UNKNOWN
    };

    struct CodeFragment {
        std::string content;
        FragmentType type;
        // Used to link a placeholder to its original code
        std::string placeholder_id;
    };

    class CHTLUnifiedScanner {
    public:
        CHTLUnifiedScanner(const std::string& source, std::shared_ptr<Configuration> config);
        std::vector<CodeFragment> scan();

    private:
        std::string m_source;
        std::shared_ptr<Configuration> m_config;
        size_t m_cursor = 0;
        int m_placeholder_counter = 0;

        // Main scanning loop
        void scanSource();
        // Specific scanner for style content
        void scanStyleContent(size_t block_end, const std::string& header);
        // Specific scanner for script content
        void scanScriptContent(size_t block_end, const std::string& header);
        // Specific scanner for origin content
        void scanOriginContent(size_t block_end, const std::string& header);

        std::vector<CodeFragment> m_fragments;
    };

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H
