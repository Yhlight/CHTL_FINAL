#pragma once

#include "../Scanner/UnifiedScanner.h"
#include <string>

class CompilerDispatcher {
public:
    CompilerDispatcher() = default;
    std::string dispatch(const std::vector<CodeSnippet>& snippets);

private:
    std::string compileChtl(const std::string& source);
};
