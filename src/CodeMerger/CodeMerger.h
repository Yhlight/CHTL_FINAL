#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Takes the main HTML output, a collection of JavaScript strings, and a placeholder map,
    // and merges them into a final HTML document.
    std::string merge(
        const std::string& html_output,
        const std::vector<std::string>& js_outputs,
        const std::map<std::string, std::string>& placeholder_map
    );
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
