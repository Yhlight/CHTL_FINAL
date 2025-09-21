#include "CompilerDispatcher.h"
#include "CodeMerger.h"
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
        std::string chtl_content;
        std::string css_content;
        std::string chtl_js_content_with_placeholders;
        std::map<std::string, std::string> plain_js_fragments;

        // Step 1: Separate all fragments into their respective buckets.
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case FragmentType::CHTL:
                    chtl_content += fragment.content;
                    break;
                case FragmentType::CSS:
                    css_content += fragment.content;
                    break;
                case FragmentType::CHTL_JS:
                    chtl_js_content_with_placeholders += fragment.content;
                    break;
                case FragmentType::JS:
                    plain_js_fragments[fragment.placeholder_id] = fragment.content;
                    break;
                case FragmentType::UNKNOWN:
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
            CHTLGenerator generator(parser.getTemplateDefinitions(), parser.getNamedOrigins(), m_config);
            CompilationResult result = generator.generate(root.get(), use_doctype);

            combinedResult.html = result.html;
            combinedResult.css = result.css + css_content;
            // JS from CHTL generator (e.g. from inside script nodes) needs to be handled
            // This generated JS might itself contain CHTL_JS that needs processing.
            // For now, we assume script blocks are handled by the scanner and we add the generator's JS.
            combinedResult.js = result.js;
        } else {
            combinedResult.css = css_content;
        }

        // Step 3: Compile the CHTL_JS content.
        std::string generated_js_with_placeholders;
        if (!chtl_js_content_with_placeholders.empty()) {
            CHTL_JS::CHTLJSLexer lexer(chtl_js_content_with_placeholders);
            auto tokens = lexer.scanTokens();
            CHTL_JS::CHTLJSParser parser(tokens, chtl_js_content_with_placeholders);
            auto ast_nodes = parser.parse();
            CHTL_JS::CHTLJSGenerator generator;
            for(const auto& node : ast_nodes) {
                generated_js_with_placeholders += generator.generate(node.get());
            }
        }

        // Step 4: Merge the plain JS with the compiled CHTL_JS
        std::string merged_js = CodeMerger::mergeJavaScript(generated_js_with_placeholders, plain_js_fragments);
        combinedResult.js += merged_js;

        return combinedResult;
    }

} // namespace CHTL
