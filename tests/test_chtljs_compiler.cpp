#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include "../src/CHTLJS/CHTLJSplitter.h"
#include "../src/CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../src/CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../src/CHTLJS/CHTLJSNode/SequenceNode.h"
#include "../src/CHTLJS/CHTLJSParser/CHTLJSContext.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.cjjs>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Split the source into CHTL JS and pure JS placeholders
        CHTLJS::CHTLJSplitter splitter(source);
        CHTLJS::JsSplitResult split_result = splitter.split();

        // 2. Lex the CHTL JS string (with placeholders)
        CHTLJS::CHTLJSLexer lexer(split_result.cjs_with_placeholders);
        std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();

        // 3. Parse the tokens
        auto context = std::make_shared<CHTLJS::CHTLJSContext>();
        CHTLJS::CHTLJSParser parser(split_result.cjs_with_placeholders, tokens, context);
        std::unique_ptr<CHTLJS::SequenceNode> ast = parser.parse();

        // 4. Generate JS from the AST (will still contain pure JS placeholders)
        CHTLJS::CHTLJSGenerator generator;
        std::string js_with_placeholders = generator.generate(*ast);

        // 5. Re-substitute the pure JS back into the compiled output
        std::string final_js = js_with_placeholders;
        for (const auto& pair : split_result.js_placeholder_map) {
            size_t pos = final_js.find(pair.first);
            if (pos != std::string::npos) {
                final_js.replace(pos, pair.first.length(), pair.second);
            }
        }

        std::cout << "--- CHTL JS COMPILER OUTPUT ---" << std::endl;
        std::cout << final_js << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
