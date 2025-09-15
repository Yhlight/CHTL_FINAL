#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Merges the final HTML, JS, and CSS into a single HTML document.
    std::string merge(std::string html_output,
                      const std::vector<std::string>& js_scripts,
                      const std::vector<std::string>& css_styles);
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
