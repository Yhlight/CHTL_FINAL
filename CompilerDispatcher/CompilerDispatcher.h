#pragma once

#include "Context/ConfigurationContext.h"
#include "CHTLNode/Node.h"
#include <string>
#include <map>

struct CompilationResult {
    std::string base_html;
    std::string compiled_css;
    std::string compiled_js;
};

class CompilerDispatcher {
public:
    CompilerDispatcher(const NodeList& ast, const ConfigurationContext& config);
    CompilationResult dispatch();

private:
    const NodeList& ast;
    const ConfigurationContext& config;
};
