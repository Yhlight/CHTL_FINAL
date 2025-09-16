#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLParser/ParserContext.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [output_basename] [--default-struct]" << std::endl;
        return 1;
    }

    std::string input_filepath = argv[1];
    std::string output_basename = "output";
    bool use_default_struct = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--default-struct") {
            use_default_struct = true;
        } else {
            output_basename = arg;
        }
    }

    std::ifstream file(input_filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Setup compilation context
        CHTL::CHTLLoader loader;
        auto context = std::make_shared<CHTL::ParserContext>();

        // 2. Lex the source file
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 3. Parse the tokens into an AST
        CHTL::CHTLParser parser(source, tokens, loader, input_filepath, context);
        std::unique_ptr<CHTL::RootNode> ast = parser.parse();

        // 4. Generate the final output from the AST
        CHTL::CHTLGenerator generator(context);
        CHTL::CompilationResult result = generator.generate(*ast, use_default_struct);

        // 5. Write the output files
        std::string html_output_path = output_basename + ".html";
        std::ofstream html_file(html_output_path);
        if (!html_file.is_open()) {
            std::cerr << "Error: Could not open " << html_output_path << " for writing." << std::endl;
            return 1;
        }
        html_file << result.html;
        std::cout << "Successfully generated " << html_output_path << std::endl;

        if (!result.js.empty()) {
            std::string js_output_path = output_basename + ".js";
            std::ofstream js_file(js_output_path);
            if (!js_file.is_open()) {
                std::cerr << "Error: Could not open " << js_output_path << " for writing." << std::endl;
                return 1;
            }
            js_file << result.js;
            std::cout << "Successfully generated " << js_output_path << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
