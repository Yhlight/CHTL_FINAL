#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CompilerDispatcher.h"
#include "../CHTL/Config/Configuration.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <vector>
#include <string>

void printUsage() {
    std::cout << "Usage: chtl <input_file> [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --inline        Embed CSS and JS into the HTML file.\n";
    std::cout << "  --default-struct  Wrap the output in a basic html5 document structure.\n";
    std::cout << "  -o <output_file>  Specify the base name for output files.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string input_file;
    std::string output_base_name;
    bool inline_output = false;
    bool use_default_struct = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--inline") {
            inline_output = true;
        } else if (arg == "--default-struct") {
            use_default_struct = true;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                output_base_name = argv[++i];
            } else {
                std::cerr << "Error: -o option requires a filename." << std::endl;
                return 1;
            }
        } else {
            if (input_file.empty()) {
                input_file = arg;
            } else {
                std::cerr << "Error: Unknown or duplicate argument: " << arg << std::endl;
                printUsage();
                return 1;
            }
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        printUsage();
        return 1;
    }

    if (output_base_name.empty()) {
        size_t dot_pos = input_file.rfind('.');
        if (dot_pos != std::string::npos) {
            output_base_name = input_file.substr(0, dot_pos);
        } else {
            output_base_name = input_file;
        }
    }

    std::string source = CHTL::FileSystem::readFile(input_file);
    if (source.empty()) {
        return 1;
    }

    auto config = std::make_shared<CHTL::Configuration>();
    // TODO: Load configuration from a file if specified

    std::cout << "Compiling " << input_file << "..." << std::endl;

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    CHTL::CompilerDispatcher dispatcher(config, input_file);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    if (inline_output) {
        std::string final_html = result.html;
        size_t head_pos = final_html.find("</head>");
        if (head_pos != std::string::npos && !result.css.empty()) {
            std::string style_tag = "\n<style>\n" + result.css + "\n</style>\n";
            final_html.insert(head_pos, style_tag);
        }

        size_t body_pos = final_html.find("</body>");
        if (body_pos != std::string::npos && !result.js.empty()) {
            std::string script_tag = "\n<script>\n" + result.js + "\n</script>\n";
            final_html.insert(body_pos, script_tag);
        }

        std::string html_file = output_base_name + ".html";
        CHTL::FileSystem::writeFile(html_file, final_html);
        std::cout << "Compilation successful. Inlined output written to " << html_file << std::endl;

    } else {
        std::string html_file = output_base_name + ".html";
        CHTL::FileSystem::writeFile(html_file, result.html);
        std::cout << "HTML output: " << html_file << std::endl;

        if (!result.css.empty()) {
            std::string css_file = output_base_name + ".css";
            CHTL::FileSystem::writeFile(css_file, result.css);
            std::cout << "CSS output: " << css_file << std::endl;
        }
        if (!result.js.empty()) {
            std::string js_file = output_base_name + ".js";
            CHTL::FileSystem::writeFile(js_file, result.js);
            std::cout << "JS output: " << js_file << std::endl;
        }
        std::cout << "Compilation successful." << std::endl;
    }


    return 0;
}
