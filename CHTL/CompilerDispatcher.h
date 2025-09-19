#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "../Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>

// Forward declare Configuration to avoid circular dependency
namespace CHTL {
    class Configuration;
}

// Forward declare CHTL JS components
namespace CHTL_JS {
    class CHTLJSLexer;
    class CHTLJSParser;
    class CHTLJSGenerator;
}

namespace CHTL {

    // This will hold the final compiled output from all compilers.
    // It's the same as CompilationResult from the old test, but we'll
    // define it centrally here.
    struct FinalCompilationResult {
        std::string html;
        std::string css;
        std::string js;
    };

    class CompilerDispatcher {
    public:
        CompilerDispatcher(std::shared_ptr<Configuration> config, const std::string& filePath);
        FinalCompilationResult dispatch(const std::vector<CodeFragment>& fragments);

    private:
        std::shared_ptr<Configuration> m_config;
        std::string m_filePath;

        // It will need instances of the various compilers, or ways to create them.
        // e.g., std::unique_ptr<CHTLParser> m_chtlParser;
    };

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
