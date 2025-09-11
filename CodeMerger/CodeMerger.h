#pragma once

#include "CompilerDispatcher.h"
#include "Scanner/UnifiedScanner.h"
#include <string>

class CodeMerger {
public:
    CodeMerger(const CompilationResult& result, const ScannedContent& content);
    std::string merge();

private:
    const CompilationResult& result;
    const ScannedContent& content;
};
