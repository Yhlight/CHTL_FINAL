#include "CompilerDispatcher/CompilerDispatcher.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <input_file.chtl> [options]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -o <output_base>   Set the base name for output files (default: 'output')" << std::endl;
    std::cerr << "  --inline             Output a single HTML file with inlined CSS and JS" << std::endl;
}

void save_to_inline_file(const std::string& base_filename, const CHTL::FinalOutput& output) {
    std::string result = "<html>\n<head>\n";
    if (!output.css.empty()) {
        result += "<style>\n" + output.css + "\n</style>\n";
    }
    result += "</head>\n<body>\n";
    result += output.html;
    if (!output.js.empty()) {
        result += "<script>\n" + output.js + "\n</script>\n";
    }
    result += "</body>\n</html>";

    std::string out_filename = base_filename + ".html";
    std::ofstream out_file(out_filename);
    if (!out_file.is_open()) throw std::runtime_error("Could not open file to save HTML: " + out_filename);
    out_file << result;
    out_file.close();
    std::cout << "Successfully generated " << out_filename << std::endl;
}

void save_to_separate_files(const std::string& base_filename, const CHTL::FinalOutput& output) {
    // Save HTML
    std::string html_filename = base_filename + ".html";
    std::string css_filename = base_filename + ".css";
    std::string js_filename = base_filename + ".js";

    std::ofstream html_file(html_filename);
    if (!html_file.is_open()) throw std::runtime_error("Could not open file to save HTML: " + html_filename);

    html_file << "<html>\n<head>\n";
    if (!output.css.empty()) html_file << "<link rel=\"stylesheet\" href=\"" << css_filename << "\">\n";
    html_file << "</head>\n<body>\n" << output.html;
    if (!output.js.empty()) html_file << "<script src=\"" << js_filename << "\"></script>\n";
    html_file << "</body>\n</html>";
    html_file.close();

    // Save CSS
    if (!output.css.empty()) {
        std::ofstream css_file(css_filename);
        if (!css_file.is_open()) throw std::runtime_error("Could not open file to save CSS: " + css_filename);
        css_file << output.css;
        css_file.close();
    }

    // Save JS
    if (!output.js.empty()) {
        std::ofstream js_file(js_filename);
        if (!js_file.is_open()) throw std::runtime_error("Could not open file to save JS: " + js_filename);
        js_file << output.js;
        js_file.close();
    }
    std::cout << "Successfully generated " << base_filename << ".html and related files." << std::endl;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string input_file;
    std::string output_base = "output";
    bool inline_output = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                output_base = argv[++i];
            } else {
                std::cerr << "Error: -o option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--inline") {
            inline_output = true;
        } else {
            if (input_file.empty()) {
                input_file = arg;
            } else {
                std::cerr << "Error: More than one input file specified." << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    try {
        std::cout << "Compiling " << input_file << "..." << std::endl;
        std::string source = CHTL::FileSystem::readFile(input_file);

        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput compiled_output = dispatcher.dispatch();

        if (inline_output) {
            save_to_inline_file(output_base, compiled_output);
        } else {
            save_to_separate_files(output_base, compiled_output);
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
