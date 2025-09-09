#pragma once

#include "../CHTL/CHTLContext/Context.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CodeMerger/CodeMerger.h"
#include "../CHTL JS/CHTLJSCompiler.h"
#include <string>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher();
    std::string compile(const std::string& source);

private:
    void findAndCompileScripts(Node* node, std::string& js_buffer);

    Context m_context;
    CHTL_JS::CHTLJSCompiler m_js_compiler;
};

} // namespace CHTL
