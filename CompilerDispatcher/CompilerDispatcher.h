#pragma once

#include "UnifiedScanner.h"
#include <string>
#include <map>

struct CompilationResult {
    std::string base_html;
    std::string compiled_css;
    std::string compiled_js;
};

class CompilerDispatcher {
public:
    CompilerDispatcher(const ScannedContent& content);
    CompilationResult dispatch();

private:
    const ScannedContent& content;
};
