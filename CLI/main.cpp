#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CompilerDispatcher.h"
#include "../CHTL/Config/Configuration.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <vector>
#include <string>

#include "../Util/ZipUtil/ZipUtil.h"

void printUsage() {
    std::cout << "Usage: chtl <input_file> [options]\n";
    std::cout << "       chtl --pack-cmod <dir_path> -o <output.cmod>\n";
    std::cout << "Options:\n";
    std::cout << "  --inline        Embed CSS and JS into the HTML file.\n";
    std::cout << "  --default-struct  Wrap the output in a basic html5 document structure.\n";
    std::cout << "  -o <output_file>  Specify the base name for output files.\n";
}

// --- CMOD Packaging Logic ---
void packageCmod(const std::string& dir_path, const std::string& output_file) {
    // 1. Validate structure
    if (!CHTL::Util::FileSystem::isDirectory(dir_path + "/src") || !CHTL::Util::FileSystem::isDirectory(dir_path + "/info")) {
        std::cerr << "Error: CMOD directory must contain 'src' and 'info' subdirectories." << std::endl;
        exit(1);
    }

    // 2. Pack using ZipUtil
    std::cout << "Packaging CMOD module from '" << dir_path << "'..." << std::endl;
    if (CHTL::Util::ZipUtil::packDirectory(dir_path, output_file)) {
        std::cout << "Successfully created CMOD module: " << output_file << std::endl;
    } else {
        std::cerr << "Error: Failed to create CMOD module." << std::endl;
        exit(1);
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    // --- Argument Parsing ---
    if (std::string(argv[1]) == "--pack-cmod") {
        if (argc != 5 || std::string(argv[3]) != "-o") {
            printUsage();
            return 1;
        }
        std::string dir_path = argv[2];
        std::string output_file = argv[4];
        packageCmod(dir_path, output_file);
        return 0; // Exit after packaging
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

    // Pre-scan for a configuration block and parse it.
    const std::string config_keyword = "[Configuration]";
    size_t config_pos = source.find(config_keyword);
    if (config_pos != std::string::npos) {
        size_t opening_brace = source.find('{', config_pos);
        if (opening_brace != std::string::npos) {
            int brace_count = 1;
            size_t closing_brace = opening_brace + 1;
            while(closing_brace < source.length() && brace_count > 0) {
                if (source[closing_brace] == '{') brace_count++;
                else if (source[closing_brace] == '}') brace_count--;
                closing_brace++;
            }
            if (brace_count == 0) {
                std::string config_content = source.substr(opening_brace + 1, closing_brace - opening_brace - 2);
                config->parseFromString(config_content);
                std::cout << "Custom configuration loaded." << std::endl;
                // We should also remove the configuration block from the source
                // so the main parser doesn't see it.
                source.erase(config_pos, closing_brace - config_pos);
            }
        }
    }

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
