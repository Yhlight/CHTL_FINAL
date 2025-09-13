#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTLJS/CHTLJSLexer.h"
#include "../CHTLJS/CHTLJSParser.h"
#include "../CHTLJS/CHTLJSGenerator.h"
#include <sstream>

namespace CHTL {

std::string CompilerDispatcher::compile(const FragmentList& fragments, const std::string& initial_path) {
    std::stringstream final_html;
    std::stringstream final_js;

    CHTLLoader loader;
    auto context = std::make_shared<ParserContext>();

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            CHTLLexer lexer(fragment.content);
            std::vector<Token> tokens = lexer.scanTokens();
            CHTLParser parser(fragment.content, tokens, loader, initial_path, context);
            std::unique_ptr<RootNode> ast = parser.parse();
            CHTLGenerator generator;
            final_html << generator.generate(*ast);
        } else if (fragment.type == FragmentType::JavaScript) {
            CHTLJS::CHTLJSLexer js_lexer(fragment.content);
            auto js_tokens = js_lexer.scanTokens();
            CHTLJS::CHTLJSParser js_parser(std::move(js_tokens));
            auto js_ast = js_parser.parse();

            // The dispatcher needs to handle file loading from ScriptLoaderNodes
            for (const auto& node : js_ast) {
                if (node->getType() == CHTLJS::JSNodeType::ScriptLoader) {
                    auto* loaderNode = static_cast<CHTLJS::ScriptLoaderNode*>(node.get());
                    for (const auto& path : loaderNode->paths) {
                        if (auto file_content = loader.loadFile(path, initial_path)) {
                            final_js << *file_content << "\n";
                        }
                    }
                }
            }

            CHTLJS::CHTLJSGenerator js_generator;
            final_js << js_generator.generate(js_ast);
        }
    }

    std::string html_output = final_html.str();
    std::string js_output = final_js.str();

    if (!js_output.empty()) {
        std::string script_tag = "\n<script>\n" + js_output + "\n</script>\n";
        size_t body_pos = html_output.rfind("</body>");
        if (body_pos != std::string::npos) {
            html_output.insert(body_pos, script_tag);
        } else {
            html_output += script_tag;
        }
    }

    return html_output;
}

} // namespace CHTL
