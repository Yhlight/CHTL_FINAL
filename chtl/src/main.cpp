#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

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

// Helper function to read a file into a string
std::string read_file_content(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for packaging: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Utility to package the Chtholly module from its source files
void package_chtholly_module() {
    std::cout << "Packaging 'Chtholly' module from source files..." << std::endl;
    try {
        auto module = std::make_shared<cmod_cjmod::CMODModule>("Chtholly");
        std::string info_content = read_file_content("chtl/modules/Chtholly/info/Chtholly.chtl");
        cmod_cjmod::ModuleInfoParser info_parser(info_content);
        module->setInfo(info_parser.parse());
        module->addSourceFile("src/Chtholly.chtl", read_file_content("chtl/modules/Chtholly/src/Chtholly.chtl"));
        module->addSourceFile("src/components/Accordion.chtl", read_file_content("chtl/modules/Chtholly/src/components/Accordion.chtl"));
        module->addSourceFile("src/components/Memo.chtl", read_file_content("chtl/modules/Chtholly/src/components/Memo.chtl"));
        std::string packed_content = cmod_cjmod::ModulePackager::pack(*module);
        std::ofstream outfile("modules/Chtholly.cmod");
        outfile << packed_content;
        outfile.close();
        std::cout << "Successfully packaged module to modules/Chtholly.cmod" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during packaging: " << e.what() << std::endl;
    }
}

// Utility to package the Yuigahama module from its source files
void package_yuigahama_module() {
    std::cout << "Packaging 'Yuigahama' module from source files..." << std::endl;
    try {
        auto module = std::make_shared<cmod_cjmod::CMODModule>("Yuigahama");
        std::string info_content = read_file_content("chtl/modules/Yuigahama/info/Yuigahama.chtl");
        cmod_cjmod::ModuleInfoParser info_parser(info_content);
        module->setInfo(info_parser.parse());
        module->addSourceFile("src/Yuigahama.chtl", read_file_content("chtl/modules/Yuigahama/src/Yuigahama.chtl"));
        module->addSourceFile("src/components/MusicPlayer.chtl", read_file_content("chtl/modules/Yuigahama/src/components/MusicPlayer.chtl"));
        module->addSourceFile("src/components/SakuraRain.chtl", read_file_content("chtl/modules/Yuigahama/src/components/SakuraRain.chtl"));
        std::string packed_content = cmod_cjmod::ModulePackager::pack(*module);
        std::ofstream outfile("modules/Yuigahama.cmod");
        outfile << packed_content;
        outfile.close();
        std::cout << "Successfully packaged module to modules/Yuigahama.cmod" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during packaging: " << e.what() << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> | --package-chtholly | --package-yuigahama" << std::endl;
        return 1;
    }

    std::string arg1 = argv[1];
    if (arg1 == "--package-chtholly") {
        package_chtholly_module();
        return 0;
    }
    if (arg1 == "--package-yuigahama") {
        package_yuigahama_module();
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
        scanner::UnifiedScanner scanner(content);
        auto fragments = scanner.scan();

        std::vector<std::string> html_parts;
        std::vector<std::string> css_parts;
        std::vector<std::string> js_parts;
        ast::ASTNode::NodePtr main_ast = nullptr;

        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case scanner::FragmentType::CHTL: {
                    lexer::CHTLLexer lexer(fragment.content);
                    auto tokens = lexer.tokenize();
                    parser::CHTLParser parser(tokens);
                    main_ast = parser.parse();
                    if (main_ast) {
                        html_parts.push_back(fragment.content);
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
                    break;
            }
        }

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
            html_parts[0] = main_ast->to_html();
        }

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