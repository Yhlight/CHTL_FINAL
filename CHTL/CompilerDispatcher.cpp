#include "CompilerDispatcher.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "Config/Configuration.h"

// CHTL JS Includes - will be needed later
// #include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
// #include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
// #include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"


namespace CHTL {

    CompilerDispatcher::CompilerDispatcher(std::shared_ptr<Configuration> config, const std::string& filePath)
        : m_config(config), m_filePath(filePath) {}

    FinalCompilationResult CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
        FinalCompilationResult combinedResult;
        std::string chtl_content;
        std::string css_content;
        std::string js_content;

        // Step 1: Concatenate all fragments of the same type.
        // This ensures that the CHTL parser, for example, sees the whole picture
        // and can correctly handle templates defined in one fragment and used in another.
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case FragmentType::CHTL:
                    chtl_content += fragment.content;
                    break;
                case FragmentType::CSS:
                    css_content += fragment.content;
                    break;
                case FragmentType::JS:
                case FragmentType::CHTL_JS: // For now, treat CHTL_JS as plain JS
                    js_content += fragment.content;
                    break;
                case FragmentType::UNKNOWN:
                    // Decide how to handle unknown fragments. For now, we can ignore them or log a warning.
                    break;
            }
        }

        // Step 2: Compile the concatenated CHTL content.
        if (!chtl_content.empty()) {
            CHTLLexer lexer(chtl_content, m_config);
            auto tokens = lexer.scanTokens();

            CHTLParser parser(chtl_content, tokens, m_filePath, m_config);
            auto root = parser.parse();
            bool use_doctype = parser.getUseHtml5Doctype();

            // The generator needs the template definitions collected by the parser.
            CHTLGenerator generator(parser.getTemplateDefinitions(), m_config);
            CompilationResult result = generator.generate(root.get(), use_doctype);

            combinedResult.html = result.html;
            // The generator might also produce CSS and JS (from style/script blocks),
            // so we should prepend this to the content we already have.
            combinedResult.css = result.css + css_content;
            combinedResult.js = result.js + js_content;
        } else {
            // If there's no CHTL, just pass through the other content.
            combinedResult.css = css_content;
            combinedResult.js = js_content;
        }

        // Step 3: In the future, compile the concatenated CSS and JS content here.
        // For now, we are just passing it through.

        return combinedResult;
    }

} // namespace CHTL
