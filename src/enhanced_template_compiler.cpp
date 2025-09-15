#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

class EnhancedTemplateCompiler {
public:
    struct TemplateInfo {
        std::string name;
        std::string type;  // "style", "element", "var"
        std::string content;
        std::map<std::string, std::string> properties;
        std::vector<std::string> inherit_from;
        std::map<std::string, std::string> specializations;
        std::set<std::string> constraints;
    };
    
    struct CustomInfo {
        std::string name;
        std::string type;  // "style", "element", "var"
        std::string content;
        std::map<std::string, std::string> properties;
        std::vector<std::string> inherit_from;
        std::map<std::string, std::string> specializations;
        std::set<std::string> constraints;
        std::map<std::string, std::string> unvalued_attributes;
    };
    
    struct CompilationContext {
        std::map<std::string, TemplateInfo> templates;
        std::map<std::string, CustomInfo> customs;
        std::map<std::string, std::string> variables;
        std::vector<std::string> imports;
        std::string current_namespace;
        std::map<std::string, std::string> configurations;
    };
    
    std::string compile(const std::string& input) {
        context_ = CompilationContext();
        std::string output;
        
        // 按行处理
        std::istringstream iss(input);
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        
        // 第一遍：收集模板和自定义信息
        for (const auto& currentLine : lines) {
            processTemplateDeclaration(currentLine);
            processCustomDeclaration(currentLine);
        }
        
        // 第二遍：处理继承和特例化
        processInheritance();
        processSpecializations();
        
        // 第三遍：生成HTML
        for (const auto& currentLine : lines) {
            std::string processedLine = processLine(currentLine);
            output += processedLine + "\n";
        }
        
        return output;
    }
    
private:
    CompilationContext context_;
    
    void processTemplateDeclaration(const std::string& line) {
        // 处理[Template] @Style
        std::regex styleRegex(R"(\[Template\]\s*@Style\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch styleMatch;
        if (std::regex_search(line, styleMatch, styleRegex)) {
            TemplateInfo info;
            info.name = styleMatch[1].str();
            info.type = "style";
            info.content = styleMatch[3].str();
            
            // 处理继承
            if (styleMatch[2].matched) {
                std::string inheritStr = styleMatch[2].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            // 解析CSS属性
            parseCSSProperties(info.content, info.properties);
            context_.templates[info.name] = info;
        }
        
        // 处理[Template] @Element
        std::regex elementRegex(R"(\[Template\]\s*@Element\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch elementMatch;
        if (std::regex_search(line, elementMatch, elementRegex)) {
            TemplateInfo info;
            info.name = elementMatch[1].str();
            info.type = "element";
            info.content = elementMatch[3].str();
            
            // 处理继承
            if (elementMatch[2].matched) {
                std::string inheritStr = elementMatch[2].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            context_.templates[info.name] = info;
        }
        
        // 处理[Template] @Var
        std::regex varRegex(R"(\[Template\]\s*@Var\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch varMatch;
        if (std::regex_search(line, varMatch, varRegex)) {
            TemplateInfo info;
            info.name = varMatch[1].str();
            info.type = "var";
            info.content = varMatch[3].str();
            
            // 处理继承
            if (varMatch[2].matched) {
                std::string inheritStr = varMatch[2].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            context_.templates[info.name] = info;
        }
    }
    
    void processCustomDeclaration(const std::string& line) {
        // 处理[Custom] Style Group
        std::regex customStyleRegex(R"(\[Custom\]\s+(\w+)\s+Style\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch customStyleMatch;
        if (std::regex_search(line, customStyleMatch, customStyleRegex)) {
            CustomInfo info;
            info.name = customStyleMatch[2].str();
            info.type = "style";
            info.content = customStyleMatch[4].str();
            
            // 处理继承
            if (customStyleMatch[3].matched) {
                std::string inheritStr = customStyleMatch[3].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            // 解析CSS属性
            parseCSSProperties(info.content, info.properties);
            context_.customs[info.name] = info;
        }
        
        // 处理[Custom] Element
        std::regex customElementRegex(R"(\[Custom\]\s+(\w+)\s+Element\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch customElementMatch;
        if (std::regex_search(line, customElementMatch, customElementRegex)) {
            CustomInfo info;
            info.name = customElementMatch[2].str();
            info.type = "element";
            info.content = customElementMatch[4].str();
            
            // 处理继承
            if (customElementMatch[3].matched) {
                std::string inheritStr = customElementMatch[3].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            context_.customs[info.name] = info;
        }
        
        // 处理[Custom] Var Group
        std::regex customVarRegex(R"(\[Custom\]\s+(\w+)\s+Var\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch customVarMatch;
        if (std::regex_search(line, customVarMatch, customVarRegex)) {
            CustomInfo info;
            info.name = customVarMatch[2].str();
            info.type = "var";
            info.content = customVarMatch[4].str();
            
            // 处理继承
            if (customVarMatch[3].matched) {
                std::string inheritStr = customVarMatch[3].str();
                parseInheritance(inheritStr, info.inherit_from);
            }
            
            context_.customs[info.name] = info;
        }
    }
    
    void parseInheritance(const std::string& inheritStr, std::vector<std::string>& inheritFrom) {
        std::istringstream iss(inheritStr);
        std::string item;
        
        while (std::getline(iss, item, ',')) {
            // 去除空白字符
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            
            if (!item.empty()) {
                inheritFrom.push_back(item);
            }
        }
    }
    
    void processInheritance() {
        // 处理模板继承
        for (auto& templatePair : context_.templates) {
            TemplateInfo& info = templatePair.second;
            if (!info.inherit_from.empty()) {
                mergeInheritedProperties(info);
            }
        }
        
        // 处理自定义继承
        for (auto& customPair : context_.customs) {
            CustomInfo& info = customPair.second;
            if (!info.inherit_from.empty()) {
                mergeInheritedCustomProperties(info);
            }
        }
    }
    
    void mergeInheritedProperties(TemplateInfo& info) {
        for (const auto& inheritName : info.inherit_from) {
            auto it = context_.templates.find(inheritName);
            if (it != context_.templates.end()) {
                const TemplateInfo& parent = it->second;
                
                // 合并属性
                for (const auto& prop : parent.properties) {
                    if (info.properties.find(prop.first) == info.properties.end()) {
                        info.properties[prop.first] = prop.second;
                    }
                }
                
                // 合并约束
                for (const auto& constraint : parent.constraints) {
                    info.constraints.insert(constraint);
                }
            }
        }
    }
    
    void mergeInheritedCustomProperties(CustomInfo& info) {
        for (const auto& inheritName : info.inherit_from) {
            auto it = context_.customs.find(inheritName);
            if (it != context_.customs.end()) {
                const CustomInfo& parent = it->second;
                
                // 合并属性
                for (const auto& prop : parent.properties) {
                    if (info.properties.find(prop.first) == info.properties.end()) {
                        info.properties[prop.first] = prop.second;
                    }
                }
                
                // 合并约束
                for (const auto& constraint : parent.constraints) {
                    info.constraints.insert(constraint);
                }
            }
        }
    }
    
    void processSpecializations() {
        // 处理模板特例化
        for (auto& templatePair : context_.templates) {
            TemplateInfo& info = templatePair.second;
            if (!info.specializations.empty()) {
                applySpecializations(info);
            }
        }
        
        // 处理自定义特例化
        for (auto& customPair : context_.customs) {
            CustomInfo& info = customPair.second;
            if (!info.specializations.empty()) {
                applyCustomSpecializations(info);
            }
        }
    }
    
    void applySpecializations(TemplateInfo& info) {
        // 应用特例化属性
        for (const auto& spec : info.specializations) {
            info.properties[spec.first] = spec.second;
        }
    }
    
    void applyCustomSpecializations(CustomInfo& info) {
        // 应用特例化属性
        for (const auto& spec : info.specializations) {
            info.properties[spec.first] = spec.second;
        }
    }
    
    void parseCSSProperties(const std::string& css, std::map<std::string, std::string>& properties) {
        std::istringstream iss(css);
        std::string line;
        
        while (std::getline(iss, line)) {
            // 去除空白字符
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.find(':') == std::string::npos) {
                continue;
            }
            
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string property = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // 去除空白字符
                property.erase(0, property.find_first_not_of(" \t"));
                property.erase(property.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (!property.empty() && !value.empty()) {
                    properties[property] = value;
                }
            }
        }
    }
    
    std::string processLine(const std::string& line) {
        std::string result = line;
        
        // 处理[Template] @Style
        if (result.find("[Template]") != std::string::npos && result.find("@Style") != std::string::npos) {
            result = processStyleTemplate(result);
        }
        // 处理[Template] @Element
        else if (result.find("[Template]") != std::string::npos && result.find("@Element") != std::string::npos) {
            result = processElementTemplate(result);
        }
        // 处理[Template] @Var
        else if (result.find("[Template]") != std::string::npos && result.find("@Var") != std::string::npos) {
            result = processVarTemplate(result);
        }
        // 处理[Custom]
        else if (result.find("[Custom]") != std::string::npos) {
            result = processCustom(result);
        }
        // 处理[Import]
        else if (result.find("[Import]") != std::string::npos) {
            result = processImport(result);
        }
        // 处理[Namespace]
        else if (result.find("[Namespace]") != std::string::npos) {
            result = processNamespace(result);
        }
        // 处理[Origin]
        else if (result.find("[Origin]") != std::string::npos) {
            result = processOrigin(result);
        }
        // 处理[Configuration]
        else if (result.find("[Configuration]") != std::string::npos) {
            result = processConfiguration(result);
        }
        
        return result;
    }
    
    std::string processStyleTemplate(const std::string& line) {
        std::regex styleRegex(R"(\[Template\]\s*@Style\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, styleRegex)) {
            std::string className = match[1].str();
            
            // 查找模板信息
            auto it = context_.templates.find(className);
            if (it != context_.templates.end()) {
                std::ostringstream oss;
                oss << "<style class=\"" << className << "\">";
                
                for (const auto& prop : it->second.properties) {
                    oss << prop.first << ": " << prop.second << "; ";
                }
                
                oss << "</style>";
                return oss.str();
            }
        }
        
        return line;
    }
    
    std::string processElementTemplate(const std::string& line) {
        std::regex elementRegex(R"(\[Template\]\s*@Element\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, elementRegex)) {
            std::string className = match[1].str();
            std::string content = match[3].str();
            
            return "<div class=\"" + className + "\">" + content + "</div>";
        }
        
        return line;
    }
    
    std::string processVarTemplate(const std::string& line) {
        std::regex varRegex(R"(\[Template\]\s*@Var\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, varRegex)) {
            std::string varName = match[1].str();
            std::string varValue = match[3].str();
            
            return "<!-- Variable: " + varName + " = " + varValue + " -->";
        }
        
        return line;
    }
    
    std::string processCustom(const std::string& line) {
        std::regex customRegex(R"(\[Custom\]\s+(\w+)\s+(\w+)\s+(\w+)(?:\s+inherit\s+([^\{]+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, customRegex)) {
            std::string groupName = match[1].str();
            std::string type = match[2].str();
            std::string className = match[3].str();
            std::string content = match[5].str();
            
            return "<div class=\"custom-" + className + "\">" + content + "</div>";
        }
        
        return line;
    }
    
    std::string processImport(const std::string& line) {
        std::regex importRegex(R"(\[Import\]\s+([^\]]+))");
        std::smatch match;
        
        if (std::regex_search(line, match, importRegex)) {
            std::string importPath = match[1].str();
            context_.imports.push_back(importPath);
            return "<!-- Import: " + importPath + " -->";
        }
        
        return line;
    }
    
    std::string processNamespace(const std::string& line) {
        std::regex namespaceRegex(R"(\[Namespace\]\s+([^\]]+))");
        std::smatch match;
        
        if (std::regex_search(line, match, namespaceRegex)) {
            std::string namespaceName = match[1].str();
            context_.current_namespace = namespaceName;
            return "<!-- Namespace: " + namespaceName + " -->";
        }
        
        return line;
    }
    
    std::string processOrigin(const std::string& line) {
        std::regex originRegex(R"(\[Origin\]\s+@(\w+)\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, originRegex)) {
            std::string type = match[1].str();
            std::string content = match[2].str();
            
            return content;
        }
        
        return line;
    }
    
    std::string processConfiguration(const std::string& line) {
        std::regex configRegex(R"(\[Configuration\]\s+([^\]]+))");
        std::smatch match;
        
        if (std::regex_search(line, match, configRegex)) {
            std::string config = match[1].str();
            return "<!-- Configuration: " + config + " -->";
        }
        
        return line;
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
        EnhancedTemplateCompiler compiler;
        std::string output = compiler.compile(content);
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << output;
        out_file.close();
        
        std::cout << "Enhanced template compilation successful. Output written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}