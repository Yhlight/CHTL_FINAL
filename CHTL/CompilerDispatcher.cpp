#include "CompilerDispatcher.h"
#include "CodeMerger.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/Visitor.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "ScriptContentProcessor.h"

// CHTL JS Includes (will be needed by the walker)
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"

namespace CHTL {

    CompilerDispatcher::CompilerDispatcher(std::shared_ptr<Configuration> config, const std::string& filePath)
        : m_config(config), m_filePath(filePath) {}

    FinalCompilationResult CompilerDispatcher::dispatch(const std::string& source) {
        // Step 1: Lex the source code into tokens.
        CHTLLexer lexer(source, m_config);
        auto tokens = lexer.scanTokens();

        // Step 2: Parse the tokens into an Abstract Syntax Tree (AST).
        CHTLParser parser(source, tokens, m_filePath, m_config);
        auto root = parser.parse();
        bool use_doctype = parser.getUseHtml5Doctype();

        // Step 3: Generate code from the AST.
        CHTLGenerator generator(parser.getMutableTemplateDefinitions(), m_config);
        CompilationResult chtl_result = generator.generate(root.get(), use_doctype);

        FinalCompilationResult result;
        result.html = chtl_result.html;
        result.css = chtl_result.css;

        // Step 4: Process the collected JavaScript content.
        if (!chtl_result.js.empty()) {
            ScriptContentProcessor processor(m_config);
            result.js = processor.process(chtl_result.js);
        } else {
            result.js = "";
        }

        return result;
    }

} // namespace CHTL
