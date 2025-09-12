#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <map>
#include <functional>

class CHTLJSCompiler {
public:
    struct CJMODInfo {
        std::string name;
        std::string content;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> dependencies;
    };
    
    struct AnimationInfo {
        std::string name;
        std::string content;
        std::string duration;
        std::string easing;
        std::string direction;
        std::string loop;
        std::string delay;
        std::map<std::string, std::string> keyframes;
    };
    
    struct RouteInfo {
        std::string path;
        std::string content;
        std::string component;
        std::map<std::string, std::string> params;
        std::vector<RouteInfo> children;
    };
    
    struct CompilationContext {
        std::map<std::string, CJMODInfo> cjmods;
        std::map<std::string, AnimationInfo> animations;
        std::map<std::string, RouteInfo> routes;
        std::map<std::string, std::string> variables;
        std::vector<std::string> imports;
        std::string current_namespace;
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
        
        // 第一遍：收集CJMOD和配置
        for (const auto& currentLine : lines) {
            processCJMODDeclaration(currentLine);
        }
        
        // 第二遍：生成JavaScript
        for (const auto& currentLine : lines) {
            std::string processedLine = processLine(currentLine);
            output += processedLine + "\n";
        }
        
        return output;
    }
    
private:
    CompilationContext context_;
    
    void processCJMODDeclaration(const std::string& line) {
        // 处理CJMOD声明
        std::regex cjmodRegex(R"(\[CJMOD\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch cjmodMatch;
        if (std::regex_search(line, cjmodMatch, cjmodRegex)) {
            CJMODInfo info;
            info.name = cjmodMatch[1].str();
            info.content = cjmodMatch[2].str();
            context_.cjmods[info.name] = info;
        }
        
        // 处理动画声明
        std::regex animRegex(R"(\[Animation\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch animMatch;
        if (std::regex_search(line, animMatch, animRegex)) {
            AnimationInfo info;
            info.name = animMatch[1].str();
            info.content = animMatch[2].str();
            parseAnimationProperties(info.content, info);
            context_.animations[info.name] = info;
        }
        
        // 处理路由声明
        std::regex routeRegex(R"(\[Route\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch routeMatch;
        if (std::regex_search(line, routeMatch, routeRegex)) {
            RouteInfo info;
            info.path = routeMatch[1].str();
            info.content = routeMatch[2].str();
            parseRouteProperties(info.content, info);
            context_.routes[info.path] = info;
        }
    }
    
    void parseAnimationProperties(const std::string& content, AnimationInfo& info) {
        std::istringstream iss(content);
        std::string line;
        
        while (std::getline(iss, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            if (line.find("duration:") != std::string::npos) {
                info.duration = extractValue(line, "duration:");
            } else if (line.find("easing:") != std::string::npos) {
                info.easing = extractValue(line, "easing:");
            } else if (line.find("direction:") != std::string::npos) {
                info.direction = extractValue(line, "direction:");
            } else if (line.find("loop:") != std::string::npos) {
                info.loop = extractValue(line, "loop:");
            } else if (line.find("delay:") != std::string::npos) {
                info.delay = extractValue(line, "delay:");
            } else if (line.find("keyframes:") != std::string::npos) {
                parseKeyframes(line, info.keyframes);
            }
        }
    }
    
    void parseRouteProperties(const std::string& content, RouteInfo& info) {
        std::istringstream iss(content);
        std::string line;
        
        while (std::getline(iss, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            if (line.find("component:") != std::string::npos) {
                info.component = extractValue(line, "component:");
            } else if (line.find("params:") != std::string::npos) {
                parseParams(line, info.params);
            }
        }
    }
    
    void parseKeyframes(const std::string& line, std::map<std::string, std::string>& keyframes) {
        // 简化的关键帧解析
        std::regex keyframeRegex(R"((\d+%?)\s*:\s*([^,]+))");
        std::sregex_iterator iter(line.begin(), line.end(), keyframeRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            keyframes[match[1].str()] = match[2].str();
        }
    }
    
    void parseParams(const std::string& line, std::map<std::string, std::string>& params) {
        // 简化的参数解析
        std::regex paramRegex(R"((\w+)\s*:\s*([^,]+))");
        std::sregex_iterator iter(line.begin(), line.end(), paramRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            params[match[1].str()] = match[2].str();
        }
    }
    
    std::string extractValue(const std::string& line, const std::string& key) {
        size_t pos = line.find(key);
        if (pos != std::string::npos) {
            std::string value = line.substr(pos + key.length());
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            return value;
        }
        return "";
    }
    
    std::string processLine(const std::string& line) {
        std::string result = line;
        
        // 处理CJMOD声明
        if (result.find("[CJMOD]") != std::string::npos) {
            result = processCJMOD(result);
        }
        // 处理动画声明
        else if (result.find("[Animation]") != std::string::npos) {
            result = processAnimation(result);
        }
        // 处理路由声明
        else if (result.find("[Route]") != std::string::npos) {
            result = processRoute(result);
        }
        // 处理变量声明
        else if (result.find("$") != std::string::npos) {
            result = processVariables(result);
        }
        // 处理选择器
        else if (result.find("{{") != std::string::npos) {
            result = processSelectors(result);
        }
        // 处理事件监听器
        else if (result.find("->") != std::string::npos) {
            result = processEventListeners(result);
        }
        
        return result;
    }
    
    std::string processCJMOD(const std::string& line) {
        std::regex cjmodRegex(R"(\[CJMOD\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, cjmodRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            return "// CJMOD: " + name + "\n" + content;
        }
        
        return line;
    }
    
    std::string processAnimation(const std::string& line) {
        std::regex animRegex(R"(\[Animation\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, animRegex)) {
            std::string name = match[1].str();
            std::string content = match[2].str();
            
            std::ostringstream oss;
            oss << "// Animation: " << name << "\n";
            oss << "const " << name << " = {\n";
            oss << "  name: '" << name << "',\n";
            oss << "  duration: '1s',\n";
            oss << "  easing: 'ease-in-out',\n";
            oss << "  keyframes: {\n";
            oss << "    '0%': { opacity: 0 },\n";
            oss << "    '100%': { opacity: 1 }\n";
            oss << "  }\n";
            oss << "};\n";
            
            return oss.str();
        }
        
        return line;
    }
    
    std::string processRoute(const std::string& line) {
        std::regex routeRegex(R"(\[Route\]\s+(\w+)\s*\{([^}]+)\})");
        std::smatch match;
        
        if (std::regex_search(line, match, routeRegex)) {
            std::string path = match[1].str();
            std::string content = match[2].str();
            
            std::ostringstream oss;
            oss << "// Route: " << path << "\n";
            oss << "const route_" << path << " = {\n";
            oss << "  path: '/" << path << "',\n";
            oss << "  component: 'div',\n";
            oss << "  params: {}\n";
            oss << "};\n";
            
            return oss.str();
        }
        
        return line;
    }
    
    std::string processVariables(const std::string& line) {
        std::string result = line;
        
        // 处理响应式变量 $variable_name$
        std::regex varRegex(R"(\$(\w+)\$)");
        result = std::regex_replace(result, varRegex, "{{$1}}");
        
        return result;
    }
    
    std::string processSelectors(const std::string& line) {
        std::string result = line;
        
        // 处理增强选择器 {{CSS_selector}}
        std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
        result = std::regex_replace(result, selectorRegex, "document.querySelector('$1')");
        
        return result;
    }
    
    std::string processEventListeners(const std::string& line) {
        std::string result = line;
        
        // 处理事件监听器 element -> event: handler
        std::regex eventRegex(R"((\w+)\s*->\s*(\w+):\s*(\w+))");
        result = std::regex_replace(result, eventRegex, "$1.addEventListener('$2', $3)");
        
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = (argc > 2) ? argv[2] : "output.js";
    
    try {
        // 读取输入文件
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 编译CHTL JS代码
        CHTLJSCompiler compiler;
        std::string output = compiler.compile(content);
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << output;
        out_file.close();
        
        std::cout << "CHTL JS Compilation successful. Output written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}