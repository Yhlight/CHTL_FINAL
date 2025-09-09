#include "CHTLJS/CHTLJSParser.h"
#include "CHTLJS/CHTLJSLexer.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLJSParser::CHTLJSParser() {
}

CHTLJSParser::~CHTLJSParser() = default;

std::string CHTLJSParser::parse(const std::string& code) {
    CHTLJSLexer lexer;
    std::vector<std::string> tokens = lexer.tokenize(code);
    
    std::stringstream result;
    size_t i = 0;
    
    while (i < tokens.size()) {
        std::string token = tokens[i];
        
        // 处理CHTL JS特殊语法
        if (token == "fileloader") {
            result << parseFileLoader(tokens, i);
        } else if (token == "script") {
            result << parseScriptBlock(tokens, i);
        } else if (token == "listen") {
            result << parseListener(tokens, i);
        } else if (token == "delegate") {
            result << parseDelegate(tokens, i);
        } else if (token == "animate") {
            result << parseAnimation(tokens, i);
        } else if (token == "vir") {
            result << parseVirtualObject(tokens, i);
        } else if (token == "router") {
            result << parseRouter(tokens, i);
        } else if (token == "->") {
            result << "=>";
        } else if (token.find("{{") != std::string::npos && token.find("}}") != std::string::npos) {
            result << parseSelector(token);
        } else {
            result << token;
        }
        
        i++;
    }
    
    return result.str();
}

std::string CHTLJSParser::parseFileLoader(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// fileloader implementation\n";
    result << "function loadFile(path) {\n";
    result << "    return fetch(path).then(response => response.text());\n";
    result << "}\n";
    return result.str();
}

std::string CHTLJSParser::parseScriptBlock(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// script block\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析脚本内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseListener(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// event listener\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析监听器内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseDelegate(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// event delegation\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析委托内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseAnimation(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// animation\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析动画内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseVirtualObject(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// virtual object\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析虚拟对象内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseRouter(const std::vector<std::string>& tokens, size_t& index) {
    std::stringstream result;
    result << "// router\n";
    
    // 跳过 '{'
    if (index + 1 < tokens.size() && tokens[index + 1] == "{") {
        index++;
        // 解析路由内容直到遇到 '}'
        while (index + 1 < tokens.size() && tokens[index + 1] != "}") {
            index++;
            result << tokens[index] << " ";
        }
        if (index + 1 < tokens.size() && tokens[index + 1] == "}") {
            index++;
        }
    }
    
    return result.str();
}

std::string CHTLJSParser::parseSelector(const std::string& token) {
    // 将 {{selector}} 转换为 document.querySelector(selector)
    std::string result = token;
    size_t start = result.find("{{");
    size_t end = result.find("}}");
    
    if (start != std::string::npos && end != std::string::npos) {
        std::string selector = result.substr(start + 2, end - start - 2);
        result = result.substr(0, start) + "document.querySelector('" + selector + "')" + result.substr(end + 2);
    }
    
    return result;
}

} // namespace CHTL