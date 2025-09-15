#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger() = default;

    // Takes the main HTML output (which may contain script placeholders)
    // and a map of placeholders to their final script content. It injects
    // the scripts into a single <script> tag at the end of the body.
    std::string merge(const std::string& html_output,
                      const std::map<std::string, std::string>& scripts);
};

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
