#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CodeMerger/CodeMerger.h"


namespace CHTL {

// This struct will be returned by the dispatcher
struct FinalCompilationResult {
    std::string html;
};

class CompilerDispatcher {
public:
    CompilerDispatcher(std::string input_filepath, const std::map<std::string, std::string>& placeholder_map);

    FinalCompilationResult dispatch(const std::vector<CodeFragment>& fragments);

private:
    // These process methods are now helpers called from the main dispatch loop
    void processCssFragment(const std::string& content);
    void processChtlJsFragment(const std::string& content);
    void processJsFragment(const std::string& content);

    std::string input_filepath_;
    const std::map<std::string, std::string>& placeholder_map_;

    // CHTL Compilation Tools
    CHTLLoader loader_;
    std::shared_ptr<ParserContext> chtl_context_;
    CHTLGenerator chtl_generator_;

    // CHTLJS Compilation Tools
    std::shared_ptr<CHTLJS::CHTLJSContext> chtljs_context_;
    // CHTLJSGenerator is now created on the stack in processChtlJsFragment

    // Code Merger
    CodeMerger code_merger_;

    // Collected results for the merger
    std::stringstream compiled_css_;
    std::stringstream compiled_js_;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
