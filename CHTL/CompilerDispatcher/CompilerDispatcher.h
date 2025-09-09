#pragma once

#include "../CHTL/CHTLContext/Context.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CodeMerger/CodeMerger.h"
#include <string>

namespace CHTL {

class CompilerDispatcher {
public:
    CompilerDispatcher();
    std::string compile(const std::string& source);

private:
    Context m_context;
};

} // namespace CHTL
