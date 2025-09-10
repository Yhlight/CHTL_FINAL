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
#include "chtl/cmod_cjmod_system.h" // Needed for packaging

using namespace chtl;

// Helper function to create a dummy module file for testing
void create_dummy_module_file() {
    std::cout << "Creating dummy module 'Chtholly.cmod'..." << std::endl;
    auto chtholly_module = std::make_shared<cmod_cjmod::CMODModule>("Chtholly");
    cmod_cjmod::ModuleInfo info("Chtholly");
    info.version = "1.0.0-packaged";
    info.author = "Packager";
    info.exports.push_back("ChthollyComponent");
    chtholly_module->setInfo(info);
    chtholly_module->addSourceFile("src/Chtholly.chtl", "div { class: \"chtholly-component\"; text: \"Content from the FILE-BASED Chtholly module\"; }");

    std::string packed_chtholly = cmod_cjmod::ModulePackager::pack(*chtholly_module);

    std::ofstream outfile("modules/Chtholly.cmod");
    outfile << packed_chtholly;
    outfile.close();

    std::cout << "Dummy module written to modules/Chtholly.cmod" << std::endl;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> | --package-dummy" << std::endl;
        return 1;
    }

    std::string arg1 = argv[1];
    if (arg1 == "--package-dummy") {
        create_dummy_module_file();
        return 0;
    }

    std::string filename = arg1;
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

        // std::cout << "Found " << fragments.size() << " fragments." << std::endl;

        // Lists to hold the final generated code parts
        std::vector<std::string> html_parts;
        std::vector<std::string> css_parts;
        std::vector<std::string> js_parts;
        ast::ASTNode::NodePtr main_ast = nullptr;


        // 2. Dispatch fragments to the correct parsers
        for (const auto& fragment : fragments) {
            // std::cout << "\n--- Processing fragment of type: " << static_cast<int>(fragment.type) << " ---\n";
            // std::cout << fragment.content.substr(0, 200) << "...\n";

            switch (fragment.type) {
                case scanner::FragmentType::CHTL: {
                    lexer::CHTLLexer lexer(fragment.content);
                    auto tokens = lexer.tokenize();
                    parser::CHTLParser parser(tokens);
                    main_ast = parser.parse(); // Assume the first CHTL block is the main one

                    if (main_ast) {
                        // We run processors on the main AST after all fragments are processed
                        html_parts.push_back(fragment.content); // Store raw for now
                    }
                    break;
                }
                case scanner::FragmentType::CSS: {
                    css_parts.push_back(fragment.content);
                    break;
                }
                case scanner::FragmentType::JS: {
                    js_parts.push_back(fragment.content);
                    break;
                }
                case scanner::FragmentType::CHTL_JS: {
                    chtl_js::CHTLJSParser parser(fragment.content);
                    auto cj_ast = parser.parse();
                    if (cj_ast) {
                        js_parts.push_back(cj_ast->to_js());
                    }
                    break;
                }
                default:
                    std::cerr << "Warning: Unknown fragment type encountered." << std::endl;
                    break;
            }
        }

        // 3. Process the main CHTL AST after initial fragment parsing
        if(main_ast) {
            import::ImportManager import_manager(".");
            import::ImportResolver import_resolver(import_manager);
            import_resolver.resolve_imports(main_ast);

            style::StyleAggregator style_aggregator;
            style_aggregator.aggregate_styles(main_ast);

            analysis::ConstraintValidator validator;
            if (!validator.validate(main_ast)) {
                std::cerr << "Constraint validation failed." << std::endl;
            }
            html_parts[0] = main_ast->to_html(); // Replace raw CHTL with processed HTML
        }


        // 4. Merge the generated parts into a final HTML document
        std::stringstream final_html;
        std::string main_html_content;

        for(const auto& part : html_parts) {
            main_html_content += part;
        }

        std::stringstream css_stream;
        for(const auto& part : css_parts) {
            css_stream << part << "\n";
        }
        std::string css_content = css_stream.str();

        std::stringstream js_stream;
        for(const auto& part : js_parts) {
            js_stream << part << "\n";
        }
        std::string js_content = js_stream.str();

        size_t head_pos = main_html_content.find("</head>");
        if (head_pos != std::string::npos && !css_content.empty()) {
            main_html_content.insert(head_pos, "<style>\n" + css_content + "</style>\n");
        }

        size_t body_pos = main_html_content.find("</body>");
        if (body_pos != std::string::npos && !js_content.empty()) {
            main_html_content.insert(body_pos, "<script>\n" + js_content + "</script>\n");
        }

        if (head_pos == std::string::npos && body_pos == std::string::npos) {
            final_html << "<!DOCTYPE html>\n<html>\n<head>\n";
            final_html << "  <meta charset=\"UTF-8\">\n  <title>CHTL Output</title>\n";
            if (!css_content.empty()) {
                final_html << "<style>\n" << css_content << "</style>\n";
            }
            final_html << "</head>\n<body>\n";
            final_html << main_html_content;
            if (!js_content.empty()) {
                final_html << "<script>\n" << js_content << "</script>\n";
            }
            final_html << "</body>\n</html>";
        } else {
            final_html << main_html_content;
        }

        std::cout << "\n\n--- FINAL HTML ---\n";
        std::cout << final_html.str() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}