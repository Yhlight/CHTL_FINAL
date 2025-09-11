#pragma once

#include "CompilationContext.h"
#include <string>

namespace CHTL {

class CHTLCompiler {
public:
    CHTLCompiler();
    std::string Compile(const std::string& source, const std::string& basePath);

private:
    void ParseFile(const std::string& source, const std::string& basePath);

    CompilationContext m_context;
};

} // namespace CHTL
