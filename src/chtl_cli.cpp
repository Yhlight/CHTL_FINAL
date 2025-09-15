#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <cstring>
#include <functional>
#include <thread>
#include <chrono>

class CHTLCli {
public:
    struct Command {
        std::string name;
        std::string description;
        std::function<int(const std::vector<std::string>&)> handler;
        
        Command() = default;
        
        Command(const std::string& n, const std::string& d, std::function<int(const std::vector<std::string>&)> h)
            : name(n), description(d), handler(h) {}
    };
    
    CHTLCli() {
        registerCommands();
    }
    
    int run(int argc, char* argv[]) {
        if (argc < 2) {
            showHelp();
            return 1;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args;
        
        for (int i = 2; i < argc; i++) {
            args.push_back(argv[i]);
        }
        
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            return it->second.handler(args);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            showHelp();
            return 1;
        }
    }
    
private:
    std::map<std::string, Command> commands_;
    
    void registerCommands() {
        commands_["compile"] = Command("compile", "Compile CHTL file to HTML", 
            [this](const std::vector<std::string>& args) { return compileCommand(args); });
        commands_["compile-js"] = Command("compile-js", "Compile CHTL JS file to JavaScript", 
            [this](const std::vector<std::string>& args) { return compileJSCommand(args); });
        commands_["init"] = Command("init", "Initialize a new CHTL project", 
            [this](const std::vector<std::string>& args) { return initCommand(args); });
        commands_["build"] = Command("build", "Build entire CHTL project", 
            [this](const std::vector<std::string>& args) { return buildCommand(args); });
        commands_["serve"] = Command("serve", "Start development server", 
            [this](const std::vector<std::string>& args) { return serveCommand(args); });
        commands_["watch"] = Command("watch", "Watch files for changes and recompile", 
            [this](const std::vector<std::string>& args) { return watchCommand(args); });
        commands_["clean"] = Command("clean", "Clean build artifacts", 
            [this](const std::vector<std::string>& args) { return cleanCommand(args); });
        commands_["help"] = Command("help", "Show help information", 
            [this](const std::vector<std::string>& args) { return helpCommand(args); });
    }
    
    void showHelp() {
        std::cout << "CHTL CLI - Command Line Interface for CHTL\n\n";
        std::cout << "Usage: chtl <command> [options]\n\n";
        std::cout << "Commands:\n";
        
        for (const auto& cmd : commands_) {
            std::cout << "  " << cmd.second.name << "    " << cmd.second.description << "\n";
        }
        
        std::cout << "\nFor more information about a specific command, use:\n";
        std::cout << "  chtl <command> --help\n";
    }
    
    int compileCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: chtl compile <input_file> [output_file]\n";
            return 1;
        }
        
        std::string input_file = args[0];
        std::string output_file = args.size() > 1 ? args[1] : "output.html";
        
        // 调用CHTL编译器
        std::string command = "./build/working_chtl_compiler " + input_file + " " + output_file;
        int result = system(command.c_str());
        
        if (result == 0) {
            std::cout << "Compilation successful: " << input_file << " -> " << output_file << std::endl;
        } else {
            std::cerr << "Compilation failed" << std::endl;
        }
        
        return result;
    }
    
    int compileJSCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: chtl compile-js <input_file> [output_file]\n";
            return 1;
        }
        
        std::string input_file = args[0];
        std::string output_file = args.size() > 1 ? args[1] : "output.js";
        
        // 调用CHTL JS编译器
        std::string command = "./build/chtl_js_compiler " + input_file + " " + output_file;
        int result = system(command.c_str());
        
        if (result == 0) {
            std::cout << "CHTL JS compilation successful: " << input_file << " -> " << output_file << std::endl;
        } else {
            std::cerr << "CHTL JS compilation failed" << std::endl;
        }
        
        return result;
    }
    
    int initCommand(const std::vector<std::string>& args) {
        std::string project_name = args.empty() ? "chtl-project" : args[0];
        
        std::cout << "Initializing CHTL project: " << project_name << std::endl;
        
        // 创建项目目录
        std::filesystem::create_directories(project_name);
        std::filesystem::create_directories(project_name + "/src");
        std::filesystem::create_directories(project_name + "/build");
        std::filesystem::create_directories(project_name + "/assets");
        
        // 创建示例文件
        createExampleFiles(project_name);
        
        std::cout << "Project initialized successfully!" << std::endl;
        std::cout << "Run 'cd " << project_name << " && chtl build' to build the project" << std::endl;
        
        return 0;
    }
    
    int buildCommand(const std::vector<std::string>& args) {
        std::cout << "Building CHTL project..." << std::endl;
        
        // 查找所有CHTL文件
        std::vector<std::string> chtl_files = findFiles(".chtl");
        std::vector<std::string> chtljs_files = findFiles(".chtljs");
        
        int success_count = 0;
        int total_count = chtl_files.size() + chtljs_files.size();
        
        // 编译CHTL文件
        for (const auto& file : chtl_files) {
            std::string output = "build/" + file.substr(0, file.find_last_of('.')) + ".html";
            std::string command = "./build/working_chtl_compiler " + file + " " + output;
            
            if (system(command.c_str()) == 0) {
                success_count++;
                std::cout << "✓ " << file << " -> " << output << std::endl;
            } else {
                std::cerr << "✗ Failed to compile " << file << std::endl;
            }
        }
        
        // 编译CHTL JS文件
        for (const auto& file : chtljs_files) {
            std::string output = "build/" + file.substr(0, file.find_last_of('.')) + ".js";
            std::string command = "./build/chtl_js_compiler " + file + " " + output;
            
            if (system(command.c_str()) == 0) {
                success_count++;
                std::cout << "✓ " << file << " -> " << output << std::endl;
            } else {
                std::cerr << "✗ Failed to compile " << file << std::endl;
            }
        }
        
        std::cout << "\nBuild completed: " << success_count << "/" << total_count << " files compiled successfully" << std::endl;
        
        return (success_count == total_count) ? 0 : 1;
    }
    
    int serveCommand(const std::vector<std::string>& args) {
        std::cout << "Starting CHTL development server..." << std::endl;
        std::cout << "Server will be available at http://localhost:8080" << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        
        // 简化的开发服务器
        std::string command = "python3 -m http.server 8080 --directory build";
        return system(command.c_str());
    }
    
    int watchCommand(const std::vector<std::string>& args) {
        std::cout << "Watching for file changes..." << std::endl;
        std::cout << "Press Ctrl+C to stop watching" << std::endl;
        
        // 简化的文件监视器
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // 这里应该实现实际的文件监视逻辑
        }
        
        return 0;
    }
    
    int cleanCommand(const std::vector<std::string>& args) {
        std::cout << "Cleaning build artifacts..." << std::endl;
        
        // 清理构建目录
        if (std::filesystem::exists("build")) {
            std::filesystem::remove_all("build");
            std::filesystem::create_directories("build");
        }
        
        std::cout << "Build artifacts cleaned successfully!" << std::endl;
        return 0;
    }
    
    int helpCommand(const std::vector<std::string>& args) {
        showHelp();
        return 0;
    }
    
    void createExampleFiles(const std::string& project_name) {
        // 创建示例CHTL文件
        std::ofstream index_file(project_name + "/src/index.chtl");
        index_file << R"(<!DOCTYPE html>
<html>
<head>
    <title>CHTL Project</title>
</head>
<body>
    <h1>Welcome to CHTL!</h1>
    
    [Template] @Style MyButton {
        background-color: #007bff;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 5px;
    }
    
    [Template] @Element ButtonContainer {
        <button class="MyButton">Click Me</button>
    }
    
    [Custom] SpecialDiv {
        <p>This is a special div with custom styling</p>
    }
    
    [Origin] @Html {
        <div class="container">
            <h2>Hello, World!</h2>
            <div class="ButtonContainer"></div>
            <div class="SpecialDiv"></div>
        </div>
    }
</body>
</html>)";
        index_file.close();
        
        // 创建示例CHTL JS文件
        std::ofstream script_file(project_name + "/src/script.chtljs");
        script_file << R"(// CHTL JS Example

[CJMOD] printHello {
    function printHello() {
        console.log("Hello from CHTL JS!");
    }
}

// 变量声明
$userName$ = "CHTL User"

// 选择器使用
const button = {{.my-button}}

// 事件监听器
button -> click: printHello

console.log("Welcome, $userName$!")";
        script_file.close();
        
        // 创建README文件
        std::ofstream readme_file(project_name + "/README.md");
        readme_file << R"(# CHTL Project

This is a CHTL project created with the CHTL CLI.

## Getting Started

1. Build the project:
   ```bash
   chtl build
   ```

2. Start the development server:
   ```bash
   chtl serve
   ```

3. Open your browser and go to http://localhost:8080

## Project Structure

- `src/` - Source files (.chtl, .chtljs)
- `build/` - Compiled output files
- `assets/` - Static assets

## Available Commands

- `chtl compile <file>` - Compile a single file
- `chtl compile-js <file>` - Compile a CHTL JS file
- `chtl build` - Build entire project
- `chtl serve` - Start development server
- `chtl watch` - Watch for changes and recompile
- `chtl clean` - Clean build artifacts
)";
        readme_file.close();
    }
    
    std::vector<std::string> findFiles(const std::string& extension) {
        std::vector<std::string> files;
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                files.push_back(entry.path().string());
            }
        }
        
        return files;
    }
};

int main(int argc, char* argv[]) {
    CHTLCli cli;
    return cli.run(argc, argv);
}