#include "CompilerDispatcher.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "Config/Configuration.h"

// CHTL JS Includes
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"


namespace CHTL {

    CompilerDispatcher::CompilerDispatcher(std::shared_ptr<Configuration> config, const std::string& filePath)
        : m_config(config), m_filePath(filePath) {}

    FinalCompilationResult CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
        FinalCompilationResult combinedResult;

        for (const auto& fragment : fragments) {
            if (fragment.type == FragmentType::CHTL) {
                // This logic is mostly lifted from the old test file and will be the
                // foundation for handling CHTL fragments.
                CHTLLexer lexer(fragment.content, m_config);
                auto tokens = lexer.scanTokens();

                CHTLParser parser(fragment.content, tokens, m_filePath, m_config);
                auto root = parser.parse();
                bool use_doctype = parser.getUseHtml5Doctype();

                CHTLGenerator generator(parser.getTemplateDefinitions(), m_config);
                CompilationResult result = generator.generate(root.get(), use_doctype);

                // For now, we just assign. Later this will be an append/merge operation.
                combinedResult.html += result.html;
                combinedResult.css += result.css;
                combinedResult.js += result.js;
            }
            else if (fragment.type == FragmentType::CHTL_JS) {
                CHTL_JS::CHTLJSLexer lexer(fragment.content);
                auto tokens = lexer.scanTokens();

                // Pass the fragment content as the source string
                CHTL_JS::CHTLJSParser parser(tokens, fragment.content);
                auto ast_nodes = parser.parse();

                CHTL_JS::CHTLJSGenerator generator;
                // Loop through all top-level statements/expressions
                for(const auto& node : ast_nodes) {
                    combinedResult.js += generator.generate(node.get());
                }
            }
            // else if (fragment.type == FragmentType::JS) { ... }
            // else if (fragment.type == FragmentType::CSS) { ... }
        }

        return combinedResult;
    }

} // namespace CHTL
