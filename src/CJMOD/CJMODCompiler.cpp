#include "CHTL/CJMOD/CJMODCompiler.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CJMODCompiler::CJMODCompiler(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_api()
    , m_debugMode(false)
    , m_compilationCount(0)
    , m_successfulCompilations(0)
    , m_failedCompilations(0)
{
}

CJMODCompiler::~CJMODCompiler() {
}

std::string CJMODCompiler::compile(const std::string& sourceCode) {
    m_compilationCount++;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling CHTL JS code..." << std::endl;
    }
    
    // 这里应该先进行词法分析，然后调用compile(TokenList)
    // 为了简化，我们直接处理源代码
    std::ostringstream result;
    
    // 简单的行处理
    std::istringstream iss(sourceCode);
    std::string line;
    while (std::getline(iss, line)) {
        // 去除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) {
            result << "\n";
            continue;
        }
        
        // 检查是否是CHTL JS关键字
        if (line.find("vir ") == 0) {
            // 处理vir语句
            std::string element = line.substr(4);
            result << "// vir: " << element << "\n";
            result << "const element = document.createElement('" << element << "');\n";
        } else if (line.find("listen ") == 0) {
            // 处理listen语句
            std::string event = line.substr(7);
            result << "// listen: " << event << "\n";
            result << "element.addEventListener('" << event << "', function(e) {\n";
            result << "    // Event handler\n";
            result << "});\n";
        } else if (line.find("animate ") == 0) {
            // 处理animate语句
            std::string properties = line.substr(8);
            result << "// animate: " << properties << "\n";
            result << "element.animate(" << properties << ");\n";
        } else if (line.find("router ") == 0) {
            // 处理router语句
            std::string route = line.substr(7);
            result << "// router: " << route << "\n";
            result << "router.navigate('" << route << "');\n";
        } else if (line.find("delegate ") == 0) {
            // 处理delegate语句
            std::string selector = line.substr(9);
            result << "// delegate: " << selector << "\n";
            result << "document.addEventListener('click', function(e) {\n";
            result << "    if (e.target.matches('" << selector << "')) {\n";
            result << "        // Delegate handler\n";
            result << "    }\n";
            result << "});\n";
        } else if (line.find("fileloader ") == 0) {
            // 处理fileloader语句
            std::string url = line.substr(11);
            result << "// fileloader: " << url << "\n";
            result << "fetch('" << url << "')\n";
            result << "    .then(response => response.text())\n";
            result << "    .then(data => {\n";
            result << "        // Handle loaded data\n";
            result << "    });\n";
        } else {
            // 普通JavaScript代码
            result << line << "\n";
        }
    }
    
    m_successfulCompilations++;
    return result.str();
}

std::string CJMODCompiler::compile(const TokenList& tokens) {
    m_compilationCount++;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling TokenList..." << std::endl;
    }
    
    std::ostringstream result;
    size_t pos = 0;
    
    while (pos < tokens.size()) {
        const Token& token = tokens[pos];
        
        switch (token.getType()) {
            case TokenType::VIR:
                result << compileVIR(tokens, pos);
                break;
            case TokenType::LISTEN:
                result << compileLISTEN(tokens, pos);
                break;
            case TokenType::ANIMATE:
                result << compileANIMATE(tokens, pos);
                break;
            case TokenType::ROUTER:
                result << compileROUTER(tokens, pos);
                break;
            case TokenType::DELEGATE:
                result << compileDELEGATE(tokens, pos);
                break;
            case TokenType::FILELOADER:
                result << compileFILELOADER(tokens, pos);
                break;
            default:
                // 普通Token，直接输出
                result << token.getValue();
                pos++;
                break;
        }
    }
    
    m_successfulCompilations++;
    return result.str();
}

void CJMODCompiler::reset() {
    m_compilationCount = 0;
    m_successfulCompilations = 0;
    m_failedCompilations = 0;
}

void CJMODCompiler::setDebugMode(bool debug) {
    m_debugMode = debug;
    m_api.setDebugMode(debug);
}

bool CJMODCompiler::isDebugMode() const {
    return m_debugMode;
}

CJMODAPI& CJMODCompiler::getAPI() {
    return m_api;
}

std::string CJMODCompiler::getStatistics() const {
    std::ostringstream oss;
    oss << "CJMOD Compiler Statistics:\n";
    oss << "  Compilation count: " << m_compilationCount << "\n";
    oss << "  Successful: " << m_successfulCompilations << "\n";
    oss << "  Failed: " << m_failedCompilations << "\n";
    oss << "  Success rate: " << (m_compilationCount > 0 ? (m_successfulCompilations * 100.0 / m_compilationCount) : 0) << "%\n";
    oss << m_api.getStatistics();
    return oss.str();
}

std::string CJMODCompiler::compileVIR(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling VIR statement" << std::endl;
    }
    
    // 跳过vir关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取元素名
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
        std::string elementName = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {elementName};
        
        // 检查是否有属性
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::LEFT_BRACE) {
            pos++; // 跳过 {
            skipWhitespace(tokens, pos);
            
            std::ostringstream attributes;
            while (pos < tokens.size() && tokens[pos].getType() != TokenType::RIGHT_BRACE) {
                if (tokens[pos].getType() == TokenType::IDENTIFIER) {
                    std::string key = tokens[pos].getValue();
                    pos++;
                    
                    if (pos < tokens.size() && tokens[pos].getType() == TokenType::COLON) {
                        pos++; // 跳过 :
                        skipWhitespace(tokens, pos);
                        
                        if (pos < tokens.size()) {
                            std::string value = tokens[pos].getValue();
                            pos++;
                            attributes << key << "=\"" << value << "\" ";
                        }
                    }
                }
                skipWhitespace(tokens, pos);
            }
            
            if (pos < tokens.size() && tokens[pos].getType() == TokenType::RIGHT_BRACE) {
                pos++; // 跳过 }
            }
            
            args.push_back(attributes.str());
        }
        
        // 调用API
        std::string virResult = m_api.callFunction("vir", args);
        result << virResult << "\n";
    }
    
    return result.str();
}

std::string CJMODCompiler::compileLISTEN(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling LISTEN statement" << std::endl;
    }
    
    // 跳过listen关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取事件名
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
        std::string eventName = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {eventName};
        
        // 检查是否有处理器
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
            std::string handler = tokens[pos].getValue();
            pos++;
            args.push_back(handler);
        }
        
        // 调用API
        std::string listenResult = m_api.callFunction("listen", args);
        result << listenResult << "\n";
    }
    
    return result.str();
}

std::string CJMODCompiler::compileANIMATE(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling ANIMATE statement" << std::endl;
    }
    
    // 跳过animate关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取元素选择器
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::STRING) {
        std::string selector = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {selector};
        
        // 检查是否有属性
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::LEFT_BRACE) {
            pos++; // 跳过 {
            skipWhitespace(tokens, pos);
            
            std::ostringstream properties;
            while (pos < tokens.size() && tokens[pos].getType() != TokenType::RIGHT_BRACE) {
                if (tokens[pos].getType() == TokenType::IDENTIFIER) {
                    std::string key = tokens[pos].getValue();
                    pos++;
                    
                    if (pos < tokens.size() && tokens[pos].getType() == TokenType::COLON) {
                        pos++; // 跳过 :
                        skipWhitespace(tokens, pos);
                        
                        if (pos < tokens.size()) {
                            std::string value = tokens[pos].getValue();
                            pos++;
                            properties << key << ": " << value << ", ";
                        }
                    }
                }
                skipWhitespace(tokens, pos);
            }
            
            if (pos < tokens.size() && tokens[pos].getType() == TokenType::RIGHT_BRACE) {
                pos++; // 跳过 }
            }
            
            args.push_back(properties.str());
        }
        
        // 调用API
        std::string animateResult = m_api.callFunction("animate", args);
        result << animateResult << "\n";
    }
    
    return result.str();
}

std::string CJMODCompiler::compileROUTER(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling ROUTER statement" << std::endl;
    }
    
    // 跳过router关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取路由路径
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::STRING) {
        std::string route = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {route};
        
        // 调用API
        std::string routerResult = m_api.callFunction("navigate", args);
        result << routerResult << "\n";
    }
    
    return result.str();
}

std::string CJMODCompiler::compileDELEGATE(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling DELEGATE statement" << std::endl;
    }
    
    // 跳过delegate关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取选择器
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::STRING) {
        std::string selector = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {selector};
        
        // 检查是否有事件类型
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
            std::string event = tokens[pos].getValue();
            pos++;
            args.push_back(event);
        }
        
        // 检查是否有处理器
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
            std::string handler = tokens[pos].getValue();
            pos++;
            args.push_back(handler);
        }
        
        // 调用API
        std::string delegateResult = m_api.callFunction("delegate", args);
        result << delegateResult << "\n";
    }
    
    return result.str();
}

std::string CJMODCompiler::compileFILELOADER(const TokenList& tokens, size_t& pos) {
    std::ostringstream result;
    
    if (m_debugMode) {
        std::cout << "[CJMODCompiler] Compiling FILELOADER statement" << std::endl;
    }
    
    // 跳过fileloader关键字
    pos++;
    skipWhitespace(tokens, pos);
    
    // 获取URL
    if (pos < tokens.size() && tokens[pos].getType() == TokenType::STRING) {
        std::string url = tokens[pos].getValue();
        pos++;
        
        // 解析参数
        std::vector<std::string> args = {url};
        
        // 检查是否有类型
        if (pos < tokens.size() && tokens[pos].getType() == TokenType::IDENTIFIER) {
            std::string type = tokens[pos].getValue();
            pos++;
            args.push_back(type);
        }
        
        // 调用API
        std::string fileloaderResult = m_api.callFunction("load", args);
        result << fileloaderResult << "\n";
    }
    
    return result.str();
}

std::vector<std::string> CJMODCompiler::parseParameters(const TokenList& tokens, size_t& pos) {
    std::vector<std::string> params;
    
    skipWhitespace(tokens, pos);
    
    while (pos < tokens.size()) {
        const Token& token = tokens[pos];
        
        if (token.getType() == TokenType::RIGHT_PAREN) {
            pos++;
            break;
        }
        
        if (token.getType() == TokenType::COMMA) {
            pos++;
            skipWhitespace(tokens, pos);
            continue;
        }
        
        params.push_back(token.getValue());
        pos++;
        skipWhitespace(tokens, pos);
    }
    
    return params;
}

void CJMODCompiler::skipWhitespace(const TokenList& tokens, size_t& pos) {
    while (pos < tokens.size() && 
           (tokens[pos].getType() == TokenType::COMMENT || 
            tokens[pos].getValue().empty() || 
            tokens[pos].getValue().find_first_not_of(" \t\n\r") == std::string::npos)) {
        pos++;
    }
}

bool CJMODCompiler::checkToken(const TokenList& tokens, size_t pos, TokenType type) {
    return pos < tokens.size() && tokens[pos].getType() == type;
}

const Token& CJMODCompiler::getCurrentToken(const TokenList& tokens, size_t pos) {
    static Token emptyToken(TokenType::UNKNOWN, "", 0, 0);
    return pos < tokens.size() ? tokens[pos] : emptyToken;
}

void CJMODCompiler::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("CJMODCompiler Error: " + message);
    }
}

void CJMODCompiler::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("CJMODCompiler Warning: " + message);
    }
}

} // namespace CHTL