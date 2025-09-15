#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
namespace CHTLJS {
    class CHTLJSContext;
    class CHTLJSParser;
    class CHTLJSGenerator;
}

namespace CHTL {

class ParserContext;
class CHTLParser;
class CHTLGenerator;
class CodeMerger;

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher(); // Required for unique_ptr to incomplete types

    std::string compile(const std::string& source);

private:
    // Contexts
    std::shared_ptr<ParserContext> chtl_context_;
    std::shared_ptr<CHTLJS::CHTLJSContext> chtljs_context_;

    // Compilers and Generators
    std::unique_ptr<CHTLParser> chtl_parser_;
    std::unique_ptr<CHTLGenerator> chtl_generator_;
    std::unique_ptr<CHTLJS::CHTLJSParser> chtljs_parser_;
    std::unique_ptr<CHTLJS::CHTLJSGenerator> chtljs_generator_;

    // Merger
    std::unique_ptr<CodeMerger> code_merger_;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
