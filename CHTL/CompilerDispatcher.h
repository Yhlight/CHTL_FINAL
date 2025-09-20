#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include "Config/Configuration.h"
#include "CHTLNode/BaseNode.h"

// Forward declare CHTL JS components
namespace CHTL_JS {
    class CHTLJSLexer;
    class CHTLJSParser;
    class CHTLJSGenerator;
}

namespace CHTL {

    // This will hold the final compiled output from all compilers.
    struct FinalCompilationResult {
        std::string html;
        std::string css;
        std::string js;
    };

    // New class to walk the AST and orchestrate compilation
    class AstWalker {
    public:
        AstWalker(std::shared_ptr<Configuration> config);
        FinalCompilationResult walk(BaseNode* root);
        // ... more methods to handle different node types
    private:
        std::shared_ptr<Configuration> m_config;
        FinalCompilationResult m_result;
    };


    class CompilerDispatcher {
    public:
        CompilerDispatcher(std::shared_ptr<Configuration> config, const std::string& filePath);
        FinalCompilationResult dispatch(const std::string& source);

    private:
        std::shared_ptr<Configuration> m_config;
        std::string m_filePath;
    };

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H
