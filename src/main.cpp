#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLParser/ParserContext.h"
#include "CodeMerger/CodeMerger.h"

// A helper function to check if a string is empty or just whitespace
bool isBlank(const std::string& str) {
    return str.find_first_not_of(" \t\n\r") == std::string::npos;
}

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
        // 1. Scan the source code to separate CHTL (with placeholders) from JS
        CHTL::CHTLUnifiedScanner scanner(source);
        CHTL::ScannedData scanned_data = scanner.scan();

        // 2. Compile the CHTL part
        CHTL::CHTLLoader loader;
        auto context = std::make_shared<CHTL::ParserContext>();
        CHTL::CHTLLexer lexer(scanned_data.chtl_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(scanned_data.chtl_source, tokens, loader, input_filepath, context);
        std::unique_ptr<CHTL::RootNode> ast = parser.parse();

        // 3. Generate HTML and JS (with placeholders) from the CHTL AST
        CHTL::CHTLGenerator generator(context);
        CHTL::CompilationResult gen_result = generator.generate(*ast, use_default_struct);

        // 4. Merge the generated JS with the original JS from the placeholder map
        CHTL::CodeMerger merger;
        std::string final_js = merger.substitutePlaceholders(gen_result.js_with_placeholders, scanned_data.js_placeholder_map);

        // 5. Combine all CSS parts
        std::string final_css = gen_result.css;
        for (const auto& css_frag : scanned_data.css_fragments) {
            final_css += "\n" + css_frag.content;
        }

        // 6. Assemble the final HTML output
        std::string final_html = gen_result.html;
        if (!use_default_struct) {
            // If not using a default structure, inject style and script tags manually.
            if (!isBlank(final_css)) {
                final_html = "<style>\n" + final_css + "</style>\n" + final_html;
            }
            if (!isBlank(final_js)) {
                final_html += "\n<script>\n" + final_js + "</script>\n";
            }
        }
        // If using default structure, the generator already placed the tags.
        // We just need to fill them if they were empty before.
        else {
             if (!isBlank(final_css)) {
                size_t pos = final_html.find("</head>");
                if (pos != std::string::npos) {
                    final_html.insert(pos, "  <style>\n" + final_css + "  </style>\n");
                }
            }
             if (!isBlank(final_js)) {
                size_t pos = final_html.find("</body>");
                 if (pos != std::string::npos) {
                    final_html.insert(pos, "  <script>\n" + final_js + "  </script>\n");
                }
            }
        }


        // 7. Write the output files
        std::string html_output_path = output_basename + ".html";
        std::ofstream html_file(html_output_path);
        html_file << final_html;
        std::cout << "Successfully generated " << html_output_path << std::endl;

        if (!isBlank(final_js)) {
            std::string js_output_path = output_basename + ".js";
            std::ofstream js_file(js_output_path);
            js_file << final_js;
            std::cout << "Successfully generated " << js_output_path << std::endl;
        }
         if (!isBlank(final_css)) {
            std::string css_output_path = output_basename + ".css";
            std::ofstream css_file(css_output_path);
            css_file << final_css;
            std::cout << "Successfully generated " << css_output_path << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
