#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

class SimpleCHTLCompiler {
public:
    std::string compile(const std::string& input) {
        std::string output;
        
        // 简单的CHTL到HTML转换
        output = convertCHTLToHTML(input);
        
        return output;
    }
    
private:
    std::string convertCHTLToHTML(const std::string& input) {
        std::string result = input;
        
        // 转换[Template] @Style 为 <style>
        std::regex templateStyleRegex(R"(\[Template\]\s*@Style\s+(\w+)\s*\{([^}]+)\})");
        result = std::regex_replace(result, templateStyleRegex, 
            R"(<style class="$1">$2</style>)");
        
        // 转换[Template] @Element 为 <div>
        std::regex templateElementRegex(R"(\[Template\]\s*@Element\s+(\w+)\s*\{([^}]+)\})");
        result = std::regex_replace(result, templateElementRegex, 
            R"(<div class="$1">$2</div>)");
        
        // 转换[Template] @Var 为变量
        std::regex templateVarRegex(R"(\[Template\]\s*@Var\s+(\w+)\s*\{([^}]+)\})");
        result = std::regex_replace(result, templateVarRegex, 
            R"(<!-- Variable: $1 = $2 -->)");
        
        // 转换[Custom] 为自定义元素
        std::regex customRegex(R"(\[Custom\]\s+(\w+)\s*\{([^}]+)\})");
        result = std::regex_replace(result, customRegex, 
            R"(<div class="custom-$1">$2</div>)");
        
        // 转换[Import] 为注释
        std::regex importRegex(R"(\[Import\]\s+([^\]]+))");
        result = std::regex_replace(result, importRegex, 
            R"(<!-- Import: $1 -->)");
        
        // 转换[Namespace] 为注释
        std::regex namespaceRegex(R"(\[Namespace\]\s+([^\]]+))");
        result = std::regex_replace(result, namespaceRegex, 
            R"(<!-- Namespace: $1 -->)");
        
        // 转换[Origin] 为原始内容
        std::regex originRegex(R"(\[Origin\]\s+@(\w+)\s*\{([^}]+)\})");
        result = std::regex_replace(result, originRegex, 
            R"($2)");
        
        // 转换[Configuration] 为注释
        std::regex configRegex(R"(\[Configuration\]\s+([^\]]+))");
        result = std::regex_replace(result, configRegex, 
            R"(<!-- Configuration: $1 -->)");
        
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // 读取输入文件
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 编译CHTL代码
        SimpleCHTLCompiler compiler;
        std::string output = compiler.compile(content);
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << output;
        out_file.close();
        
        std::cout << "Compilation successful. Output written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}