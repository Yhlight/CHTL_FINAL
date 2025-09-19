#pragma once

#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"
#include <vector>
#include <map>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher(CodeMerger& merger);
    void dispatch(const ScanResult& scan_result, const std::string& entry_point);

private:
    CodeMerger& merger;
    std::map<std::string, std::string> compiled_blocks;
};

} // namespace CHTL
