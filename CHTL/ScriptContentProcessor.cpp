#include "ScriptContentProcessor.h"
#include "CodeMerger.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <vector>
#include <map>

namespace CHTL {

// This list should be kept in sync with any CHTL JS language additions.
// It's a simplified detection mechanism. A more robust solution might
// involve a more integrated parser.
const std::vector<std::string> CHTLJS_KEYWORDS = {
    "Listen", "Delegate", "Animate", "Router", "Vir", "printMylove", "iNeverAway", "util"
    };

ScriptContentProcessor::ScriptContentProcessor(std::shared_ptr<Configuration> config)
    : m_config(config) {}

std::string ScriptContentProcessor::process(const std::string& script_content) {
    if (script_content.empty()) {
        return "";
    }

    std::map<std::string, std::string> js_placeholders;
    std::string chtl_js_with_placeholders;
    std::string js_buffer;
    size_t cursor = 0;

    auto finalize_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            std::string placeholder_id = "_JS_PLACEHOLDER_" + std::to_string(m_placeholder_counter++) + "_";
            js_placeholders[placeholder_id] = js_buffer;
            chtl_js_with_placeholders += placeholder_id;
            js_buffer.clear();
        }
    };

    while (cursor < script_content.length()) {
        bool is_chtl_js_construct = false;

        // Check for {{ ... }}
        if (script_content.substr(cursor, 2) == "{{") {
            size_t end_pos = script_content.find("}}", cursor);
            if (end_pos != std::string::npos) {
                finalize_js_buffer();
                chtl_js_with_placeholders += script_content.substr(cursor, end_pos + 2 - cursor);
                cursor = end_pos + 2;
                is_chtl_js_construct = true;
            }
        } else {
            // Check for CHTL JS keywords
            for (const auto& keyword : CHTLJS_KEYWORDS) {
                if (script_content.substr(cursor, keyword.length()) == keyword) {
                     bool is_standalone = (cursor == 0 || !std::isalnum(script_content[cursor - 1]));
                     if (is_standalone) {
                        finalize_js_buffer();
                        // This is a simplification. We assume the keyword is followed by a space
                        // and then a block or expression. We will just consume the keyword.
                        // A more robust parser would be needed for complex cases.
                        chtl_js_with_placeholders += keyword;
                        cursor += keyword.length();
                        is_chtl_js_construct = true;
                        break;
                     }
                }
            }
        }

        if (!is_chtl_js_construct) {
            js_buffer += script_content[cursor];
            cursor++;
        }
    }

    finalize_js_buffer(); // Add any remaining JS at the end

    // Step 2: Compile the CHTL_JS content.
    std::string generated_js_with_placeholders;
    if (!chtl_js_with_placeholders.empty()) {
        CHTL_JS::CHTLJSLexer lexer(chtl_js_with_placeholders);
        auto tokens = lexer.scanTokens();
        CHTL_JS::CHTLJSParser parser(tokens, chtl_js_with_placeholders);
        auto ast_nodes = parser.parse();
        CHTL_JS::CHTLJSGenerator generator;
        for(const auto& node : ast_nodes) {
            generated_js_with_placeholders += generator.generate(node.get());
        }
    }

    // Step 3: Merge the plain JS with the compiled CHTL_JS
    return CodeMerger::mergeJavaScript(generated_js_with_placeholders, js_placeholders);
}


} // namespace CHTL
