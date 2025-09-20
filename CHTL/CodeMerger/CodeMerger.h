#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include "../CompilerDispatcher/CompilerDispatcher.h" // For FinalOutput
#include <string>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger(const FinalOutput& output);

    // Merges everything into a single string (for inline HTML)
    std::string mergeToSingleFile() const;

    // Merges with only CSS inlined
    std::string mergeCssInline() const;

    // Saves the output to separate files (e.g., "output.html", "output.css")
    void saveToSeparateFiles(const std::string& base_filename) const;

    // Saves the output with inlined CSS
    void saveCssInlined(const std::string& base_filename) const;

private:
    const FinalOutput& output;
};

} // namespace CHTL

#endif // CODE_MERGER_H
