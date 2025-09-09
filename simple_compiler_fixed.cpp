#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>

class SimpleCHTLCompiler {
private:
    std::string m_input;
    std::string m_output;
    bool m_defaultStruct;
    
public:
    SimpleCHTLCompiler() : m_defaultStruct(false) {}
    
    void setDefaultStruct(bool enable) {
        m_defaultStruct = enable;
    }
    
    bool compile(const std::string& inputFile, const std::string& outputFile) {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "错误: 无法打开文件 " << inputFile << std::endl;
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_input = buffer.str();
        file.close();
        
        // 编译 CHTL 代码
        m_output = compileCHTL(m_input);
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "错误: 无法创建文件 " << outputFile << std::endl;
            return false;
        }
        
        outFile << m_output;
        outFile.close();
        
        std::cout << "编译完成: " << inputFile << " -> " << outputFile << std::endl;
        return true;
    }
    
private:
    std::string compileCHTL(const std::string& input) {
        std::string result;
        
        if (m_defaultStruct) {
            result += "<!DOCTYPE html>\n";
            result += "<html>\n";
            result += "<head>\n";
            result += "  <meta charset=\"UTF-8\">\n";
            result += "  <title>CHTL Generated Page</title>\n";
            result += "</head>\n";
            result += "<body>\n";
        }
        
        // 简化的 CHTL 编译
        result += processCHTL(input);
        
        if (m_defaultStruct) {
            result += "</body>\n";
            result += "</html>\n";
        }
        
        return result;
    }
    
    std::string processCHTL(const std::string& input) {
        std::string result = input;
        
        // 移除注释
        result = std::regex_replace(result, std::regex("//.*$"), "", std::regex_constants::match_default);
        result = std::regex_replace(result, std::regex("/\\*[\\s\\S]*?\\*/"), "");
        result = std::regex_replace(result, std::regex("^--.*$"), "", std::regex_constants::match_default);
        
        // 处理模板
        result = processTemplates(result);
        
        // 处理自定义
        result = processCustoms(result);
        
        // 处理原始嵌入
        result = processOrigins(result);
        
        // 处理导入
        result = processImports(result);
        
        // 处理约束
        result = processConstraints(result);
        
        // 处理配置
        result = processConfigurations(result);
        
        // 处理命名空间
        result = processNamespaces(result);
        
        // 处理元素
        result = processElements(result);
        
        // 处理样式
        result = processStyles(result);
        
        // 处理脚本
        result = processScripts(result);
        
        return result;
    }
    
    std::string processTemplates(const std::string& input) {
        std::string result = input;
        
        // 处理 [Template] 块
        std::regex templateRegex("\\[Template\\]\\s+@(\\w+)\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, templateRegex)) {
            std::string type = match[1].str();
            std::string name = match[2].str();
            std::string content = match[3].str();
            
            // 简化处理：直接替换为注释
            std::string replacement = "<!-- Template: " + type + " " + name + " -->\n" + content;
            result = std::regex_replace(result, templateRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processCustoms(const std::string& input) {
        std::string result = input;
        
        // 处理 [Custom] 块
        std::regex customRegex("\\[Custom\\]\\s+@(\\w+)\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, customRegex)) {
            std::string type = match[1].str();
            std::string name = match[2].str();
            std::string content = match[3].str();
            
            // 简化处理：直接替换为注释
            std::string replacement = "<!-- Custom: " + type + " " + name + " -->\n" + content;
            result = std::regex_replace(result, customRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processOrigins(const std::string& input) {
        std::string result = input;
        
        // 处理 [Origin] 块
        std::regex originRegex("\\[Origin\\]\\s+@(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, originRegex)) {
            std::string type = match[1].str();
            std::string content = match[2].str();
            
            // 直接输出原始内容
            result = std::regex_replace(result, originRegex, content, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processImports(const std::string& input) {
        std::string result = input;
        
        // 处理 [Import] 块
        std::regex importRegex("\\[Import\\]\\s+\"([^\"]+)\"(?:\\s+as\\s+(\\w+))?");
        std::smatch match;
        
        while (std::regex_search(result, match, importRegex)) {
            std::string file = match[1].str();
            std::string alias = match[2].str();
            
            // 简化处理：替换为注释
            std::string replacement = "<!-- Import: " + file;
            if (!alias.empty()) {
                replacement += " as " + alias;
            }
            replacement += " -->";
            
            result = std::regex_replace(result, importRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processConstraints(const std::string& input) {
        std::string result = input;
        
        // 处理 [Constraints] 块
        std::regex constraintRegex("\\[Constraints\\]\\s+except\\s+([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, constraintRegex)) {
            std::string constraint = match[1].str();
            
            // 简化处理：替换为注释
            std::string replacement = "<!-- Constraint: " + constraint + " -->";
            result = std::regex_replace(result, constraintRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processConfigurations(const std::string& input) {
        std::string result = input;
        
        // 处理 [Configuration] 块
        std::regex configRegex("\\[Configuration\\]\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, configRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            // 简化处理：替换为注释
            std::string replacement = "<!-- Configuration: " + name + " -->\n" + content;
            result = std::regex_replace(result, configRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processNamespaces(const std::string& input) {
        std::string result = input;
        
        // 处理 [Namespace] 块
        std::regex namespaceRegex("\\[Namespace\\]\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, namespaceRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            // 简化处理：替换为注释
            std::string replacement = "<!-- Namespace: " + name + " -->\n" + content;
            result = std::regex_replace(result, namespaceRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processElements(const std::string& input) {
        std::string result = input;
        
        // 处理元素：identifier { content }
        std::regex elementRegex("(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, elementRegex)) {
            std::string tag = match[1].str();
            std::string content = match[2].str();
            
            // 跳过关键字
            if (tag == "text" || tag == "style" || tag == "script" || 
                tag == "Template" || tag == "Custom" || tag == "Origin" ||
                tag == "Import" || tag == "Constraints" || tag == "Configuration" ||
                tag == "Namespace" || tag == "use") {
                continue;
            }
            
            // 处理内容
            std::string processedContent = processElementContent(content);
            
            // 生成 HTML
            std::string replacement = "<" + tag + ">\n" + processedContent + "</" + tag + ">\n";
            result = std::regex_replace(result, elementRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processElementContent(const std::string& content) {
        std::string result = content;
        
        // 处理属性：key: "value";
        std::regex attrRegex("(\\w+)\\s*:\\s*\"([^\"]+)\";");
        std::smatch match;
        
        while (std::regex_search(result, match, attrRegex)) {
            std::string key = match[1].str();
            std::string value = match[2].str();
            
            // 替换为 HTML 属性
            std::string replacement = key + "=\"" + value + "\" ";
            result = std::regex_replace(result, attrRegex, replacement, std::regex_constants::format_first_only);
        }
        
        // 处理文本节点：text { "content" }
        std::regex textRegex("text\\s*\\{\\s*\"([^\"]+)\"\\s*\\}");
        result = std::regex_replace(result, textRegex, "$1");
        
        return result;
    }
    
    std::string processStyles(const std::string& input) {
        std::string result = input;
        
        // 处理 style 块
        std::regex styleRegex("style\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, styleRegex)) {
            std::string content = match[1].str();
            
            // 生成 CSS
            std::string replacement = "<style>\n" + content + "\n</style>";
            result = std::regex_replace(result, styleRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processScripts(const std::string& input) {
        std::string result = input;
        
        // 处理 script 块
        std::regex scriptRegex("script\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, scriptRegex)) {
            std::string content = match[1].str();
            
            // 生成 JavaScript
            std::string replacement = "<script>\n" + content + "\n</script>";
            result = std::regex_replace(result, scriptRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <input.chtl> [output.html] [--default-struct]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    bool defaultStruct = false;
    
    // 检查参数
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--default-struct") {
            defaultStruct = true;
        }
    }
    
    SimpleCHTLCompiler compiler;
    compiler.setDefaultStruct(defaultStruct);
    
    if (compiler.compile(inputFile, outputFile)) {
        std::cout << "编译成功!" << std::endl;
        return 0;
    } else {
        std::cout << "编译失败!" << std::endl;
        return 1;
    }
}