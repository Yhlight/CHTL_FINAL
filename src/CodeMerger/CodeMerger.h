#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Takes HTML, CSS, and JS and merges them into a final HTML document.
    std::string merge(const std::string& html_output, const std::string& css_output, const std::vector<std::string>& js_outputs);
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
