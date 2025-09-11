#pragma once

#include "Context/ConfigurationContext.h"
#include "CHTLNode/Node.h"
#include "Scanner/UnifiedScanner.h"
#include <string>
#include <map>

struct CompilationResult {
    std::string base_html;
    std::string compiled_css; // For global CSS from imports
    std::map<std::string, std::string> compiled_scripts; // placeholder -> compiled JS
};

class CompilerDispatcher {
public:
    CompilerDispatcher(const NodeList& ast, const ScannedContent& content, const ConfigurationContext& config, const std::string& base_path);
    CompilationResult dispatch();

private:
    const NodeList& ast;
    const ScannedContent& content;
    const ConfigurationContext& config;
    const std::string& base_path;
};
