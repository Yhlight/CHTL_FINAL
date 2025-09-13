#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
// #include "../CHTLJS/CHTLJSParser.h" // Future CHTL JS Parser
// #include "../CodeMerger/CodeMerger.h" // Future Code Merger
#include <string>
#include <vector>
#include <memory>
#include "../CHTLJS/CHTLJSParser/CHTLJSContext.h"

namespace CHTL {

// Forward declarations for future components
class CHTLJSParser;
class CodeMerger;

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    std::string compile(const std::string& source);

private:
    std::vector<CodeChunk> chunks_;
    std::shared_ptr<ParserContext> chtl_context_;
    std::shared_ptr<CHTLJS::CHTLJSContext> chtljs_context_;

    // Merger
    std::unique_ptr<CodeMerger> code_merger_;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
