#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include "../CompilerDispatcher/CompilerDispatcher.h" // For CompilationResult
#include <string>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger(const CompilationResult& output);

    // Merges everything into a single string (for inline HTML)
    std::string mergeToSingleFile() const;

    // Saves the output to separate files (e.g., "output.html", "output.css")
    void saveToSeparateFiles(const std::string& base_filename) const;

private:
    const CompilationResult& output;
};

} // namespace CHTL

#endif // CODE_MERGER_H
