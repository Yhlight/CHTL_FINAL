#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <filesystem>

class ModuleSystemCompiler {
public:
    struct CMODInfo {
        std::string name;
        std::string type;  // "style", "element", "var"
        std::string content;
        std::map<std::string, std::string> properties;
        std::vector<std::string> dependencies;
        std::string namespace_;
        std::map<std::string, std::string> exports;
    };
    
    struct CJMODInfo {
        std::string name;
        std::string content;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> dependencies;
        std::string namespace_;
        std::map<std::string, std::string> exports;
    };
    
    struct ModuleContext {
        std::map<std::string, CMODInfo> cmods;
        std::map<std::string, CJMODInfo> cjmods;
        std::map<std::string, std::string> variables;
        std::vector<std::string> imports;
        std::string current_namespace;
        std::map<std::string, std::string> configurations;
        std::map<std::string, std::string> module_paths;
    };
    
    std::string compile(const std::string& input) {
        context_ = ModuleContext();
        std::string output;
        
        // 按行处理
        std::istringstream iss(input);
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        
        // 第一遍：收集模块信息
        for (const auto& currentLine : lines) {
            processModuleDeclaration(currentLine);
        }
        
        // 第二遍：处理模块依赖和导出
        processModuleDependencies();
        processModuleExports();
        
        // 第三遍：生成输出
        for (const auto& currentLine : lines) {
            std::string processedLine = processLine(currentLine);
            output += processedLine + "\n";
        }
        
        return output;
    }
    
private:
    ModuleContext context_;
    
    void processModuleDeclaration(const std::string& line) {
        // 处理CMOD声明
        std::regex cmodRegex(R"(\[CMOD\]\s+(\w+)\s+(\w+)\s+(\w+)(?:\s+namespace\s+(\w+))?\s*\{([^}]+)\})");
        std::smatch cmodMatch;
        if (std::regex_search(line, cmodMatch, cmodRegex)) {
            CMODInfo info;
            info.name = cmodMatch[3].str();
            info.type = cmodMatch[2].str();
            info.content = cmodMatch[5].str();
            info.namespace_ = cmodMatch[4].matched ? cmodMatch[4].str() : "default";
            
            // 解析内容
            if (info.type == "style") {
                parseCSSProperties(info.content, info.properties);
            }
            
            context_.cmods[info.name] = info;
        }
        
        // 处理CJMOD声明
        std::regex cjmodRegex(R"(\[CJMOD\]\s+(\w+)(?:\s+namespace\s+(\w+))?\s*\{([^}]+)\})");
        std::smatch cjmodMatch;
        if (std::regex_search(line, cjmodMatch, cjmodRegex)) {
            CJMODInfo info;
            info.name = cjmodMatch[1].str();
            info.content = cjmodMatch[3].str();
            info.namespace_ = cjmodMatch[2].matched ? cjmodMatch[2].str() : "default";
            
            // 解析参数
            parseCJMODParameters(info.content, info.parameters);
            
            context_.cjmods[info.name] = info;
        }
        
        // 处理模块导入
        std::regex importRegex(R"(\[Import\]\s+(\w+)\s+(\w+)\s+from\s+([^\]]+))");
        std::smatch importMatch;
        if (std::regex_search(line, importMatch, importRegex)) {
            std::string moduleType = importMatch[1].str();
            std::string moduleName = importMatch[2].str();
            std::string modulePath = importMatch[3].str();
            
            context_.module_paths[moduleName] = modulePath;
            context_.imports.push_back(moduleName);
        }
        
        // 处理通配符导入
        std::regex wildcardRegex(R"(\[Import\]\s+(\w+)\s+(\*)\s+from\s+([^\]]+))");
        std::smatch wildcardMatch;
        if (std::regex_search(line, wildcardMatch, wildcardRegex)) {
            std::string moduleType = wildcardMatch[1].str();
            std::string modulePath = wildcardMatch[3].str();
            
            // 处理通配符导入
            processWildcardImport(moduleType, modulePath);
        }
    }
    
    void processWildcardImport(const std::string& moduleType, const std::string& modulePath) {
        // 简化的通配符导入处理
        if (moduleType == "CMOD") {
            // 模拟导入所有CMOD模块
            context_.imports.push_back("AllCMODModules");
        } else if (moduleType == "CJMOD") {
            // 模拟导入所有CJMOD模块
            context_.imports.push_back("AllCJMODModules");
        }
    }
    
    void parseCJMODParameters(const std::string& content, std::map<std::string, std::string>& parameters) {
        std::istringstream iss(content);
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
                std::string param = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // 去除空白字符
                param.erase(0, param.find_first_not_of(" \t"));
                param.erase(param.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (!param.empty() && !value.empty()) {
                    parameters[param] = value;
                }
            }
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
    
    void processModuleDependencies() {
        // 处理CMOD依赖
        for (auto& cmodPair : context_.cmods) {
            CMODInfo& info = cmodPair.second;
            // 简化的依赖处理
            if (info.type == "style") {
                info.dependencies.push_back("base-styles");
            }
        }
        
        // 处理CJMOD依赖
        for (auto& cjmodPair : context_.cjmods) {
            CJMODInfo& info = cjmodPair.second;
            // 简化的依赖处理
            info.dependencies.push_back("base-js");
        }
    }
    
    void processModuleExports() {
        // 处理CMOD导出
        for (auto& cmodPair : context_.cmods) {
            CMODInfo& info = cmodPair.second;
            info.exports["name"] = info.name;
            info.exports["type"] = info.type;
            info.exports["namespace"] = info.namespace_;
        }
        
        // 处理CJMOD导出
        for (auto& cjmodPair : context_.cjmods) {
            CJMODInfo& info = cjmodPair.second;
            info.exports["name"] = info.name;
            info.exports["namespace"] = info.namespace_;
        }
    }
    
    std::string processLine(const std::string& line) {
        std::string result = line;
        
        // 处理CMOD声明
        if (result.find("[CMOD]") != std::string::npos) {
            result = processCMOD(result);
        }
        // 处理CJMOD声明
        else if (result.find("[CJMOD]") != std::string::npos) {
            result = processCJMOD(result);
        }
        // 处理模块导入
        else if (result.find("[Import]") != std::string::npos) {
            result = processImport(result);
        }
        // 处理命名空间
        else if (result.find("[Namespace]") != std::string::npos) {
            result = processNamespace(result);
        }
        // 处理配置
        else if (result.find("[Configuration]") != std::string::npos) {
            result = processConfiguration(result);
        }
        
        return result;
    }
    
    std::string processCMOD(const std::string& line) {
        std::regex cmodRegex(R"(\[CMOD\]\s+(\w+)\s+(\w+)\s+(\w+)(?:\s+namespace\s+(\w+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, cmodRegex)) {
            std::string moduleName = match[1].str();
            std::string type = match[2].str();
            std::string name = match[3].str();
            std::string namespace_ = match[4].matched ? match[4].str() : "default";
            std::string content = match[5].str();
            
            std::ostringstream oss;
            oss << "<!-- CMOD: " << moduleName << "." << name << " (namespace: " << namespace_ << ") -->";
            
            if (type == "style") {
                oss << "\n<style class=\"" << name << "\">";
                // 查找模块信息
                auto it = context_.cmods.find(name);
                if (it != context_.cmods.end()) {
                    for (const auto& prop : it->second.properties) {
                        oss << prop.first << ": " << prop.second << "; ";
                    }
                }
                oss << "</style>";
            } else if (type == "element") {
                oss << "\n<div class=\"" << name << "\">" << content << "</div>";
            } else if (type == "var") {
                oss << "\n<!-- Variable: " << name << " = " << content << " -->";
            }
            
            return oss.str();
        }
        
        return line;
    }
    
    std::string processCJMOD(const std::string& line) {
        std::regex cjmodRegex(R"(\[CJMOD\]\s+(\w+)(?:\s+namespace\s+(\w+))?\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, cjmodRegex)) {
            std::string name = match[1].str();
            std::string namespace_ = match[2].matched ? match[2].str() : "default";
            std::string content = match[3].str();
            
            std::ostringstream oss;
            oss << "// CJMOD: " << name << " (namespace: " << namespace_ << ")\n";
            oss << content;
            
            return oss.str();
        }
        
        return line;
    }
    
    std::string processImport(const std::string& line) {
        std::regex importRegex(R"(\[Import\]\s+(\w+)\s+(\w+)\s+from\s+([^\]]+))");
        std::smatch match;
        
        if (std::regex_search(line, match, importRegex)) {
            std::string moduleType = match[1].str();
            std::string moduleName = match[2].str();
            std::string modulePath = match[3].str();
            
            return "<!-- Import: " + moduleType + " " + moduleName + " from " + modulePath + " -->";
        }
        
        // 处理通配符导入
        std::regex wildcardRegex(R"(\[Import\]\s+(\w+)\s+(\*)\s+from\s+([^\]]+))");
        std::smatch wildcardMatch;
        if (std::regex_search(line, wildcardMatch, wildcardRegex)) {
            std::string moduleType = wildcardMatch[1].str();
            std::string modulePath = wildcardMatch[3].str();
            
            return "<!-- Wildcard Import: " + moduleType + " * from " + modulePath + " -->";
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
        
        // 编译模块系统
        ModuleSystemCompiler compiler;
        std::string output = compiler.compile(content);
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << output;
        out_file.close();
        
        std::cout << "Module system compilation successful. Output written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}