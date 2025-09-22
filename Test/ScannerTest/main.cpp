#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

void printFragment(const CHTL::CodeFragment& fragment) {
    std::string type;
    switch (fragment.type) {
        case CHTL::FragmentType::CHTL: type = "CHTL"; break;
        case CHTL::FragmentType::CHTL_JS: type = "CHTL_JS"; break;
        case CHTL::FragmentType::CSS: type = "CSS"; break;
        case CHTL::FragmentType::JS: type = "JS"; break;
        case CHTL::FragmentType::HTML: type = "HTML"; break;
        case CHTL::FragmentType::DIRECTIVE: type = "DIRECTIVE"; break;
        case CHTL::FragmentType::UNKNOWN: type = "UNKNOWN"; break;
    }
    std::cout << "--- Fragment ---" << std::endl;
    std::cout << "Type: " << type << std::endl;
    std::cout << "Content: \n" << fragment.content << std::endl;
    std::cout << "----------------" << std::endl;
}

int main() {
    std::string source = CHTL::FileSystem::readFile("Test/ScannerTest/main.chtl");
    assert(!source.empty());

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    // Optional: print fragments for debugging
    // for(const auto& frag : fragments) {
    //     printFragment(frag);
    // }

    bool found_directive = false;
    bool found_origin_html = false;
    bool found_style_content = false;
    bool found_script_js_part1 = false;
    bool found_script_chtl_js_part = false;
    bool found_script_js_part2 = false;

    for(const auto& frag : fragments) {
        if (frag.type == CHTL::FragmentType::DIRECTIVE && frag.content == "use html5;") {
            found_directive = true;
        }
        if (frag.type == CHTL::FragmentType::HTML && frag.content.find("<nav>This is raw HTML</nav>") != std::string::npos) {
            found_origin_html = true;
        }
        if (frag.type == CHTL::FragmentType::CHTL && frag.content.find("width: 100px / 2;") != std::string::npos) {
            found_style_content = true;
        }
        if (frag.type == CHTL::FragmentType::JS && frag.content.find("let a = 1;") != std::string::npos) {
            found_script_js_part1 = true;
        }
        if (frag.type == CHTL::FragmentType::CHTL_JS && frag.content.find("->Listen") != std::string::npos) {
            found_script_chtl_js_part = true;
        }
        if (frag.type == CHTL::FragmentType::JS && frag.content.find("console.log('done');") != std::string::npos) {
            found_script_js_part2 = true;
        }
    }

    assert(found_directive);
    assert(found_origin_html);
    assert(found_style_content);
    assert(found_script_js_part1);
    assert(found_script_chtl_js_part);
    assert(found_script_js_part2);

    std::cout << "ScannerTest passed!" << std::endl;

    return 0;
}
