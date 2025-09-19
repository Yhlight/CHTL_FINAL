#ifndef CHTL_CODE_MERGER_H
#define CHTL_CODE_MERGER_H

#include "CompilerDispatcher.h" // For FinalCompilationResult
#include <string>

namespace CHTL {

    class CodeMerger {
    public:
        CodeMerger();

        // In the future, this might take more context, like the placeholder map.
        std::string mergeToFinalHtml(const FinalCompilationResult& result);

    private:
        // Helper methods for merging and decoding placeholders will go here.
    };

} // namespace CHTL

#endif // CHTL_CODE_MERGER_H
