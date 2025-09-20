#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

    class CodeMerger {
    public:
        /**
         * @brief Merges a string containing placeholders with a map of the original code fragments.
         *
         * It finds placeholders (e.g., _JS_PLACEHOLDER_0_) in the input string and replaces
         * them with the corresponding content from the provided map.
         *
         * @param js_with_placeholders The string containing placeholders.
         * @param js_fragments A map where the key is the placeholder_id and the value is the original code.
         * @return The final, merged string with all placeholders replaced.
         */
        static std::string mergeJavaScript(
            const std::string& js_with_placeholders,
            const std::map<std::string, std::string>& js_fragments
        );

        /**
         * @brief Merges the final compiled HTML, CSS, and JS into a single,
         * self-contained HTML string.
         */
        static std::string mergeToFinalHtml(
            const std::string& html,
            const std::string& css,
            const std::string& js
        );
    };

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
