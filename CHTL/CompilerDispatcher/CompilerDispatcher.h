#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "../Scanner/CodeFragment.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

// A simple struct to hold the final compiled outputs
struct FinalOutput {
    std::string html;
    std::string css;
    std::string js;
    std::string imported_css;
    std::string imported_js;
    std::map<std::string, std::string> named_html_snippets;
};

class CompilerDispatcher {
public:
    // New constructor for loader integration
    explicit CompilerDispatcher(std::string root_path, std::map<std::string, TemplateDefinitionNode>& templates);


    // The main method to process fragments
    FinalOutput dispatch();

private:
    void process_source(const std::string& source, int depth);

    std::string root_path;
    // Changed to a reference to use the map from the loader
    std::map<std::string, TemplateDefinitionNode>& all_template_definitions;
    FinalOutput final_output;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H
