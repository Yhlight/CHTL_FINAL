#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Takes the main HTML output and collections of JavaScript and CSS strings
    // and merges them into a final HTML document.
    std::string merge(const std::string& html_output, const std::vector<std::string>& js_outputs, const std::vector<std::string>& css_outputs);
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
