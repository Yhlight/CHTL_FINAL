#pragma once

#include "CompilerDispatcher.h"
#include <string>

class CodeMerger {
public:
    CodeMerger(const CompilationResult& result);
    std::string merge();

private:
    const CompilationResult& result;
};
