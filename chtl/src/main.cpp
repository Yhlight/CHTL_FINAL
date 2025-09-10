#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "scanner/unified_scanner.h"
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "chtl_js/chtl_js_parser.h"
#include "ast/ast_node.h"
#include "style/style_aggregator.h"
#include "import/import_manager.h"
#include "import/import_resolver.h"
#include "analysis/constraint_validator.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    try {
        // 1. Use the UnifiedScanner to get code fragments
        scanner::UnifiedScanner scanner(content);
        auto fragments = scanner.scan();

        std::cout << "Found " << fragments.size() << " fragments." << std::endl;

        // Lists to hold the final generated code parts
        std::vector<std::string> html_parts;
        std::vector<std::string> css_parts;
        std::vector<std::string> js_parts;

        // 2. Dispatch fragments to the correct parsers
        for (const auto& fragment : fragments) {
            std::cout << "\n--- Processing fragment of type: " << static_cast<int>(fragment.type) << " ---\n";
            // std::cout << fragment.content.substr(0, 200) << "...\n";

            switch (fragment.type) {
                case scanner::FragmentType::CHTL: {
                    lexer::CHTLLexer lexer(fragment.content);
                    auto tokens = lexer.tokenize();
                    parser::CHTLParser parser(tokens);
                    auto ast = parser.parse();

                    if (ast) {
                        // We can still run the CHTL-specific processors here
                        import::ImportManager import_manager(".");
                        import::ImportResolver import_resolver(import_manager);
                        import_resolver.resolve_imports(ast);

                        style::StyleAggregator style_aggregator;
                        style_aggregator.aggregate_styles(ast);

                        analysis::ConstraintValidator validator;
                        if (!validator.validate(ast)) {
                            std::cerr << "Constraint validation failed." << std::endl;
                            // Handle errors...
                        }

                        html_parts.push_back(ast->to_html());
                    }
                    break;
                }
                case scanner::FragmentType::CSS: {
                    // For now, just collect CSS content directly
                    css_parts.push_back(fragment.content);
                    break;
                }
                case scanner::FragmentType::JS: {
                    // For now, just collect JS content directly
                    js_parts.push_back(fragment.content);
                    break;
                }
                case scanner::FragmentType::CHTL_JS: {
                    chtl_js::CHTLJSParser parser(fragment.content);
                    auto ast = parser.parse();
                    if (ast) {
                        js_parts.push_back(ast->to_js());
                    }
                    break;
                }
                default:
                    std::cerr << "Warning: Unknown fragment type encountered." << std::endl;
                    break;
            }
        }

        // 3. Merge the generated parts into a final HTML document
        std::stringstream final_html;
        std::string main_html_content;

        // Combine all HTML parts into one
        for(const auto& part : html_parts) {
            main_html_content += part;
        }

        // Combine all CSS parts
        std::stringstream css_stream;
        for(const auto& part : css_parts) {
            css_stream << part << "\n";
        }
        std::string css_content = css_stream.str();

        // Combine all JS parts
        std::stringstream js_stream;
        for(const auto& part : js_parts) {
            js_stream << part << "\n";
        }
        std::string js_content = js_stream.str();

        // Inject CSS and JS into the main HTML content
        size_t head_pos = main_html_content.find("</head>");
        if (head_pos != std::string::npos && !css_content.empty()) {
            main_html_content.insert(head_pos, "<style>\n" + css_content + "</style>\n");
        }

        size_t body_pos = main_html_content.find("</body>");
        if (body_pos != std::string::npos && !js_content.empty()) {
            main_html_content.insert(body_pos, "<script>\n" + js_content + "</script>\n");
        }

        // If no structure was found, create a boilerplate
        if (head_pos == std::string::npos && body_pos == std::string::npos) {
            final_html << "<!DOCTYPE html>\n";
            final_html << "<html>\n";
            final_html << "<head>\n";
            final_html << "  <meta charset=\"UTF-8\">\n";
            final_html << "  <title>CHTL Output</title>\n";
            if (!css_content.empty()) {
                final_html << "<style>\n" << css_content << "</style>\n";
            }
            final_html << "</head>\n";
            final_html << "<body>\n";
            final_html << main_html_content; // The content generated from CHTL
            if (!js_content.empty()) {
                final_html << "<script>\n" << js_content << "</script>\n";
            }
            final_html << "</body>\n";
            final_html << "</html>";
        } else {
            final_html << main_html_content;
        }

        // Print the final result
        std::cout << "\n\n--- FINAL HTML ---\n";
        std::cout << final_html.str() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}