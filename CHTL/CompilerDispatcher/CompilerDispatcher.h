#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "../Scanner/CodeFragment.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

// A simple struct to hold the final compiled outputs
struct CompilationResult {
    std::string html;
    std::string css;
    std::string js;
};

class CompilerDispatcher {
public:
    explicit CompilerDispatcher(std::string initial_source);

    // The main method to process fragments
    CompilationResult dispatch();

private:
    void process_source(const std::string& source, int depth);

    std::string initial_source;
    std::map<std::string, TemplateDefinitionNode> all_template_definitions;
    CompilationResult final_output;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H
