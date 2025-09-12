#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTLJS/CJMODSystem.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --debug          Enable debug mode" << std::endl;
        std::cout << "  --output <file>  Output file (default: output.html)" << std::endl;
        std::cout << "  --help           Show this help message" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = "output.html";
    bool debug_mode = false;
    
    // 解析命令行参数
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--debug") {
            debug_mode = true;
        } else if (arg == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "--help") {
            std::cout << "CHTL Compiler - A C++ based hypertext language" << std::endl;
            std::cout << "Usage: chtl <input_file> [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --debug          Enable debug mode" << std::endl;
            std::cout << "  --output <file>  Output file (default: output.html)" << std::endl;
            std::cout << "  --help           Show this help message" << std::endl;
            return 0;
        }
    }
    
    try {
        // 创建编译器上下文
        CHTLContext context;
        context.setDebugMode(debug_mode);
        
        // 创建统一扫描器
        UnifiedScanner scanner;
        scanner.setDebugMode(debug_mode);
        
        // 创建CJMOD系统
        CJMODSystem cjmod_system;
        cjmod_system.setDebugMode(debug_mode);
        
        // 读取输入文件
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string code((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
        file.close();
        
        if (debug_mode) {
            std::cout << "[Main] Input file loaded: " << input_file << std::endl;
            std::cout << "[Main] Code length: " << code.length() << " characters" << std::endl;
        }
        
        // 扫描代码，分离不同类型的代码片段
        auto fragments = scanner.scan(code);
        
        if (debug_mode) {
            std::cout << "[Main] Found " << fragments.size() << " code fragments" << std::endl;
            for (const auto& fragment : fragments) {
                std::cout << "[Main] Fragment type: " << static_cast<int>(fragment.type) 
                          << ", length: " << fragment.content.length() << std::endl;
            }
        }
        
        // 处理不同类型的代码片段
        std::string html_output;
        std::string css_output;
        std::string js_output;
        
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case UnifiedScanner::CodeType::CHTL:
                    // 处理CHTL代码
                    if (debug_mode) {
                        std::cout << "[Main] Processing CHTL fragment" << std::endl;
                    }
                    // 这里应该调用CHTL编译器
                    html_output += fragment.content;
                    break;
                    
                case UnifiedScanner::CodeType::CHTL_JS:
                    // 处理CHTL JS代码
                    if (debug_mode) {
                        std::cout << "[Main] Processing CHTL JS fragment" << std::endl;
                    }
                    // 这里应该调用CHTL JS编译器
                    js_output += cjmod_system.generateJS(fragment.content);
                    break;
                    
                case UnifiedScanner::CodeType::CSS:
                    // 处理CSS代码
                    if (debug_mode) {
                        std::cout << "[Main] Processing CSS fragment" << std::endl;
                    }
                    css_output += fragment.content;
                    break;
                    
                case UnifiedScanner::CodeType::JS:
                    // 处理JS代码
                    if (debug_mode) {
                        std::cout << "[Main] Processing JS fragment" << std::endl;
                    }
                    js_output += fragment.content;
                    break;
                    
                default:
                    if (debug_mode) {
                        std::cout << "[Main] Unknown fragment type" << std::endl;
                    }
                    break;
            }
        }
        
        // 生成最终输出
        std::string final_output = "<!DOCTYPE html>\n<html>\n<head>\n";
        
        if (!css_output.empty()) {
            final_output += "<style>\n" + css_output + "\n</style>\n";
        }
        
        final_output += "</head>\n<body>\n";
        
        if (!html_output.empty()) {
            final_output += html_output + "\n";
        }
        
        if (!js_output.empty()) {
            final_output += "<script>\n" + js_output + "\n</script>\n";
        }
        
        final_output += "</body>\n</html>\n";
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << final_output;
        out_file.close();
        
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Output written to: " << output_file << std::endl;
        
        if (debug_mode) {
            std::cout << "[Main] HTML output length: " << html_output.length() << std::endl;
            std::cout << "[Main] CSS output length: " << css_output.length() << std::endl;
            std::cout << "[Main] JS output length: " << js_output.length() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}