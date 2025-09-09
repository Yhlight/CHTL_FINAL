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
        // Read input file
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << inputFile << std::endl;
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        m_input = buffer.str();
        file.close();
        
        // Compile CHTL code
        m_output = compileCHTL(m_input);
        
        // Write output file
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create file " << outputFile << std::endl;
            return false;
        }
        
        outFile << m_output;
        outFile.close();
        
        std::cout << "Compilation completed: " << inputFile << " -> " << outputFile << std::endl;
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
        
        // Simplified CHTL compilation
        result += processCHTL(input);
        
        if (m_defaultStruct) {
            result += "</body>\n";
            result += "</html>\n";
        }
        
        return result;
    }
    
    std::string processCHTL(const std::string& input) {
        std::string result = input;
        
        // Remove comments
        result = std::regex_replace(result, std::regex("//.*$"), "", std::regex_constants::match_default);
        result = std::regex_replace(result, std::regex("/\\*[\\s\\S]*?\\*/"), "");
        result = std::regex_replace(result, std::regex("^--.*$"), "", std::regex_constants::match_default);
        
        // Process templates
        result = processTemplates(result);
        
        // Process customs
        result = processCustoms(result);
        
        // Process origins
        result = processOrigins(result);
        
        // Process imports
        result = processImports(result);
        
        // Process constraints
        result = processConstraints(result);
        
        // Process configurations
        result = processConfigurations(result);
        
        // Process namespaces
        result = processNamespaces(result);
        
        // Process elements
        result = processElements(result);
        
        // Process styles
        result = processStyles(result);
        
        // Process scripts
        result = processScripts(result);
        
        return result;
    }
    
    std::string processTemplates(const std::string& input) {
        std::string result = input;
        
        // Process [Template] blocks
        std::regex templateRegex("\\[Template\\]\\s+@(\\w+)\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, templateRegex)) {
            std::string type = match[1].str();
            std::string name = match[2].str();
            std::string content = match[3].str();
            
            // Simplified processing: replace with comment
            std::string replacement = "<!-- Template: " + type + " " + name + " -->\n" + content;
            result = std::regex_replace(result, templateRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processCustoms(const std::string& input) {
        std::string result = input;
        
        // Process [Custom] blocks
        std::regex customRegex("\\[Custom\\]\\s+@(\\w+)\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, customRegex)) {
            std::string type = match[1].str();
            std::string name = match[2].str();
            std::string content = match[3].str();
            
            // Simplified processing: replace with comment
            std::string replacement = "<!-- Custom: " + type + " " + name + " -->\n" + content;
            result = std::regex_replace(result, customRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processOrigins(const std::string& input) {
        std::string result = input;
        
        // Process [Origin] blocks
        std::regex originRegex("\\[Origin\\]\\s+@(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, originRegex)) {
            std::string type = match[1].str();
            std::string content = match[2].str();
            
            // Direct output of raw content
            result = std::regex_replace(result, originRegex, content, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processImports(const std::string& input) {
        std::string result = input;
        
        // Process [Import] blocks
        std::regex importRegex("\\[Import\\]\\s+\"([^\"]+)\"(?:\\s+as\\s+(\\w+))?");
        std::smatch match;
        
        while (std::regex_search(result, match, importRegex)) {
            std::string file = match[1].str();
            std::string alias = match[2].str();
            
            // Simplified processing: replace with comment
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
        
        // Process [Constraints] blocks
        std::regex constraintRegex("\\[Constraints\\]\\s+except\\s+([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, constraintRegex)) {
            std::string constraint = match[1].str();
            
            // Simplified processing: replace with comment
            std::string replacement = "<!-- Constraint: " + constraint + " -->";
            result = std::regex_replace(result, constraintRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processConfigurations(const std::string& input) {
        std::string result = input;
        
        // Process [Configuration] blocks
        std::regex configRegex("\\[Configuration\\]\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, configRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            // Simplified processing: replace with comment
            std::string replacement = "<!-- Configuration: " + name + " -->\n" + content;
            result = std::regex_replace(result, configRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processNamespaces(const std::string& input) {
        std::string result = input;
        
        // Process [Namespace] blocks
        std::regex namespaceRegex("\\[Namespace\\]\\s+(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, namespaceRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            // Simplified processing: replace with comment
            std::string replacement = "<!-- Namespace: " + name + " -->\n" + content;
            result = std::regex_replace(result, namespaceRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processElements(const std::string& input) {
        std::string result = input;
        
        // Process elements: identifier { content }
        std::regex elementRegex("(\\w+)\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, elementRegex)) {
            std::string tag = match[1].str();
            std::string content = match[2].str();
            
            // Skip keywords
            if (tag == "text" || tag == "style" || tag == "script" || 
                tag == "Template" || tag == "Custom" || tag == "Origin" ||
                tag == "Import" || tag == "Constraints" || tag == "Configuration" ||
                tag == "Namespace" || tag == "use") {
                continue;
            }
            
            // Process content
            std::string processedContent = processElementContent(content);
            
            // Generate HTML
            std::string replacement = "<" + tag + ">\n" + processedContent + "</" + tag + ">\n";
            result = std::regex_replace(result, elementRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processElementContent(const std::string& content) {
        std::string result = content;
        
        // Process attributes: key: "value";
        std::regex attrRegex("(\\w+)\\s*:\\s*\"([^\"]+)\";");
        std::smatch match;
        
        while (std::regex_search(result, match, attrRegex)) {
            std::string key = match[1].str();
            std::string value = match[2].str();
            
            // Replace with HTML attributes
            std::string replacement = key + "=\"" + value + "\" ";
            result = std::regex_replace(result, attrRegex, replacement, std::regex_constants::format_first_only);
        }
        
        // Process text nodes: text { "content" }
        std::regex textRegex("text\\s*\\{\\s*\"([^\"]+)\"\\s*\\}");
        result = std::regex_replace(result, textRegex, "$1");
        
        return result;
    }
    
    std::string processStyles(const std::string& input) {
        std::string result = input;
        
        // Process style blocks
        std::regex styleRegex("style\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, styleRegex)) {
            std::string content = match[1].str();
            
            // Generate CSS
            std::string replacement = "<style>\n" + content + "\n</style>";
            result = std::regex_replace(result, styleRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
    
    std::string processScripts(const std::string& input) {
        std::string result = input;
        
        // Process script blocks
        std::regex scriptRegex("script\\s*\\{([\\s\\S]*?)\\}");
        std::smatch match;
        
        while (std::regex_search(result, match, scriptRegex)) {
            std::string content = match[1].str();
            
            // Generate JavaScript
            std::string replacement = "<script>\n" + content + "\n</script>";
            result = std::regex_replace(result, scriptRegex, replacement, std::regex_constants::format_first_only);
        }
        
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [output.html] [--default-struct]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    bool defaultStruct = false;
    
    // Check arguments
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--default-struct") {
            defaultStruct = true;
        }
    }
    
    SimpleCHTLCompiler compiler;
    compiler.setDefaultStruct(defaultStruct);
    
    if (compiler.compile(inputFile, outputFile)) {
        std::cout << "Compilation successful!" << std::endl;
        return 0;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        return 1;
    }
}