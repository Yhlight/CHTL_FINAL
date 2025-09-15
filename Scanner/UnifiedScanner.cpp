#include "UnifiedScanner.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : position(0), line(1), column(1) {
}

UnifiedScanner::~UnifiedScanner() {
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    setSource(source);
    std::vector<CodeFragment> fragments;
    
    while (hasMoreCode()) {
        try {
            CodeFragment fragment = scanNextFragment();
            if (!fragment.content.empty()) {
                fragments.push_back(fragment);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return fragments;
}

void UnifiedScanner::setSource(const std::string& source) {
    this->source = source;
    reset();
}

void UnifiedScanner::reset() {
    position = 0;
    line = 1;
    column = 1;
    errors.clear();
}

bool UnifiedScanner::hasMoreCode() const {
    return position < source.length();
}

CodeFragment UnifiedScanner::scanNextFragment() {
    if (isAtEnd()) {
        return CodeFragment(CodeType::UNKNOWN, "", line, column, line, column);
    }
    
    skipWhitespace();
    
    if (isAtEnd()) {
        return CodeFragment(CodeType::UNKNOWN, "", line, column, line, column);
    }
    
    // 检测代码类型
    if (isCHTLBoundary()) {
        return scanCHTL();
    } else if (isCHTLJSBoundary()) {
        return scanCHTLJS();
    } else if (isCSSBoundary()) {
        return scanCSS();
    } else if (isJSBoundary()) {
        return scanJS();
    } else {
        // 默认为JS代码
        return scanJS();
    }
}

CodeFragment UnifiedScanner::scanCHTL() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string content;
    
    // 扫描CHTL代码块
    while (!isAtEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            content += c;
            advance();
            
            // 扫描到匹配的右括号
            int braceCount = 1;
            while (!isAtEnd() && braceCount > 0) {
                char next = currentChar();
                content += next;
                advance();
                
                if (next == '{') {
                    braceCount++;
                } else if (next == '}') {
                    braceCount--;
                } else if (next == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }
            
            if (braceCount == 0) {
                break;
            }
        } else {
            content += c;
            advance();
            
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
    }
    
    return CodeFragment(CodeType::CHTL, content, startLine, startColumn, line, column);
}

CodeFragment UnifiedScanner::scanCHTLJS() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string content;
    
    // 扫描CHTL JS代码块
    while (!isAtEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            content += c;
            advance();
            
            // 扫描到匹配的右括号
            int braceCount = 1;
            while (!isAtEnd() && braceCount > 0) {
                char next = currentChar();
                content += next;
                advance();
                
                if (next == '{') {
                    braceCount++;
                } else if (next == '}') {
                    braceCount--;
                } else if (next == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }
            
            if (braceCount == 0) {
                break;
            }
        } else {
            content += c;
            advance();
            
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
    }
    
    return CodeFragment(CodeType::CHTL_JS, content, startLine, startColumn, line, column);
}

CodeFragment UnifiedScanner::scanCSS() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string content;
    
    // 扫描CSS代码块
    while (!isAtEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            content += c;
            advance();
            
            // 扫描到匹配的右括号
            int braceCount = 1;
            while (!isAtEnd() && braceCount > 0) {
                char next = currentChar();
                content += next;
                advance();
                
                if (next == '{') {
                    braceCount++;
                } else if (next == '}') {
                    braceCount--;
                } else if (next == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }
            
            if (braceCount == 0) {
                break;
            }
        } else {
            content += c;
            advance();
            
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
    }
    
    return CodeFragment(CodeType::CSS, content, startLine, startColumn, line, column);
}

CodeFragment UnifiedScanner::scanJS() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string content;
    
    // 扫描JS代码块
    while (!isAtEnd()) {
        char c = currentChar();
        
        if (c == '{') {
            content += c;
            advance();
            
            // 扫描到匹配的右括号
            int braceCount = 1;
            while (!isAtEnd() && braceCount > 0) {
                char next = currentChar();
                content += next;
                advance();
                
                if (next == '{') {
                    braceCount++;
                } else if (next == '}') {
                    braceCount--;
                } else if (next == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
            }
            
            if (braceCount == 0) {
                break;
            }
        } else {
            content += c;
            advance();
            
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
    }
    
    return CodeFragment(CodeType::JS, content, startLine, startColumn, line, column);
}

void UnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && std::isspace(currentChar())) {
        if (currentChar() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        advance();
    }
}

void UnifiedScanner::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        while (!isAtEnd() && currentChar() != '\n') {
            advance();
        }
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // 跳过 '/'
        advance(); // 跳过 '*'
        
        while (!isAtEnd()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 '*'
                advance(); // 跳过 '/'
                break;
            }
            advance();
        }
    }
}

bool UnifiedScanner::isAtEnd() const {
    return position >= source.length();
}

char UnifiedScanner::currentChar() const {
    return isAtEnd() ? '\0' : source[position];
}

char UnifiedScanner::peekChar() const {
    return (position + 1 >= source.length()) ? '\0' : source[position + 1];
}

void UnifiedScanner::advance() {
    if (!isAtEnd()) {
        position++;
        column++;
    }
}

bool UnifiedScanner::isCHTLBoundary() const {
    // 检测CHTL语法边界
    // 例如: div { }, [Template], [Custom], 等
    std::string lookahead = source.substr(position, std::min(size_t(10), source.length() - position));
    
    // 检测HTML元素
    if (std::isalpha(lookahead[0]) && lookahead.find('{') != std::string::npos) {
        return true;
    }
    
    // 检测模板语法
    if (lookahead.find("[Template]") == 0 || lookahead.find("[Custom]") == 0 ||
        lookahead.find("[Origin]") == 0 || lookahead.find("[Import]") == 0 ||
        lookahead.find("[Configuration]") == 0 || lookahead.find("[Namespace]") == 0) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSBoundary() const {
    // 检测CHTL JS语法边界
    // 例如: Vir, Listen, Animate, Router, 等
    std::string lookahead = source.substr(position, std::min(size_t(20), source.length() - position));
    
    // 检测CHTL JS关键字
    if (lookahead.find("Vir ") == 0 || lookahead.find("Listen ") == 0 ||
        lookahead.find("Animate ") == 0 || lookahead.find("Router ") == 0 ||
        lookahead.find("ScriptLoader ") == 0 || lookahead.find("Delegate ") == 0 ||
        lookahead.find("iNeverAway ") == 0 || lookahead.find("util ") == 0) {
        return true;
    }
    
    // 检测选择器语法
    if (lookahead.find("{{") == 0) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCSSBoundary() const {
    // 检测CSS语法边界
    // 例如: .class, #id, @media, 等
    std::string lookahead = source.substr(position, std::min(size_t(10), source.length() - position));
    
    if (lookahead[0] == '.' || lookahead[0] == '#' || lookahead[0] == '@' ||
        lookahead.find("style") == 0) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isJSBoundary() const {
    // 检测JavaScript语法边界
    // 例如: function, const, let, var, 等
    std::string lookahead = source.substr(position, std::min(size_t(20), source.length() - position));
    
    if (lookahead.find("function") == 0 || lookahead.find("const") == 0 ||
        lookahead.find("let") == 0 || lookahead.find("var") == 0 ||
        lookahead.find("if") == 0 || lookahead.find("for") == 0 ||
        lookahead.find("while") == 0 || lookahead.find("class") == 0) {
        return true;
    }
    
    return false;
}

std::string UnifiedScanner::processPlaceholders(const std::string& code, CodeType type) {
    switch (type) {
        case CodeType::CHTL_JS:
            return replaceJSWithPlaceholder(code);
        case CodeType::JS:
            return replaceCHTLWithPlaceholder(code);
        default:
            return code;
    }
}

std::string UnifiedScanner::replaceJSWithPlaceholder(const std::string& code) {
    std::string result = code;
    
    // 将JS代码块替换为占位符
    size_t pos = 0;
    while ((pos = result.find("function", pos)) != std::string::npos) {
        // 找到函数开始
        size_t start = pos;
        size_t braceCount = 0;
        bool inFunction = false;
        
        while (pos < result.length()) {
            if (result[pos] == '{') {
                braceCount++;
                inFunction = true;
            } else if (result[pos] == '}') {
                braceCount--;
                if (inFunction && braceCount == 0) {
                    // 找到函数结束
                    size_t end = pos + 1;
                    std::string placeholder = "_JS_CODE_PLACEHOLDER_";
                    result.replace(start, end - start, placeholder);
                    pos = start + placeholder.length();
                    break;
                }
            }
            pos++;
        }
    }
    
    return result;
}

std::string UnifiedScanner::replaceCHTLWithPlaceholder(const std::string& code) {
    std::string result = code;
    
    // 将CHTL语法替换为占位符
    size_t pos = 0;
    while ((pos = result.find("{{", pos)) != std::string::npos) {
        size_t end = result.find("}}", pos);
        if (end != std::string::npos) {
            end += 2; // 包含 }}
            std::string placeholder = "_CHTL_CODE_PLACEHOLDER_";
            result.replace(pos, end - pos, placeholder);
            pos += placeholder.length();
        } else {
            break;
        }
    }
    
    return result;
}

void UnifiedScanner::reportError(const std::string& message) {
    reportError(line, column, message);
}

void UnifiedScanner::reportError(size_t line, size_t column, const std::string& message) {
    std::string error = "Error at line " + std::to_string(line) + 
                       ", column " + std::to_string(column) + ": " + message;
    errors.push_back(error);
}

void UnifiedScanner::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (currentChar() == ';') {
            advance();
            break;
        }
        
        if (isCHTLBoundary() || isCHTLJSBoundary() || 
            isCSSBoundary() || isJSBoundary()) {
            break;
        }
        
        advance();
    }
}

} // namespace CHTL