#include "CHTLJSParser.hpp"
#include <sstream>

namespace CHTL {

CHTLJSParser::CHTLJSParser(std::istream& input) 
    : lexer_(input), hasError_(false) {
    current_ = lexer_.nextToken();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parse() {
    auto root = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    while (current_.getType() != CHTLJSTokenType::END_OF_FILE && !hasError_) {
        if (current_.getType() == CHTLJSTokenType::FILELOADER) {
            auto fileLoader = parseFileLoader();
            if (fileLoader) {
                root->addChild(fileLoader);
            }
        } else if (current_.getType() == CHTLJSTokenType::SCRIPT) {
            auto script = parseScript();
            if (script) {
                root->addChild(script);
            }
        } else if (current_.getType() == CHTLJSTokenType::ENHANCED_SELECTOR_START) {
            auto selector = parseEnhancedSelector();
            if (selector) {
                root->addChild(selector);
            }
        } else if (current_.getType() == CHTLJSTokenType::ANIMATE) {
            auto animate = parseAnimate();
            if (animate) {
                root->addChild(animate);
            }
        } else if (current_.getType() == CHTLJSTokenType::LISTEN) {
            auto listen = parseListen();
            if (listen) {
                root->addChild(listen);
            }
        } else if (current_.getType() == CHTLJSTokenType::UTIL) {
            auto util = parseUtil();
            if (util) {
                root->addChild(util);
            }
        } else {
            // 跳过未知词法单元
            current_ = lexer_.nextToken();
        }
    }
    
    return root;
}

bool CHTLJSParser::consume(CHTLJSTokenType type) {
    if (current_.getType() == type) {
        current_ = lexer_.nextToken();
        return true;
    }
    return false;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFileLoader() {
    if (!consume(CHTLJSTokenType::FILELOADER)) {
        reportError("Expected fileloader");
        return nullptr;
    }
    
    auto fileLoader = std::make_shared<FileLoaderNode>(current_.getLine(), current_.getColumn());
    
    if (consume(CHTLJSTokenType::LEFT_BRACE)) {
        while (current_.getType() != CHTLJSTokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == CHTLJSTokenType::IDENTIFIER && 
                current_.getValue() == "load") {
                current_ = lexer_.nextToken();
                
                if (consume(CHTLJSTokenType::COLON)) {
                    if (current_.getType() == CHTLJSTokenType::STRING_LITERAL ||
                        current_.getType() == CHTLJSTokenType::UNQUOTED_LITERAL) {
                        fileLoader->addLoadFile(current_.getValue());
                        current_ = lexer_.nextToken();
                    }
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return fileLoader;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseScript() {
    if (!consume(CHTLJSTokenType::SCRIPT)) {
        reportError("Expected script");
        return nullptr;
    }
    
    auto script = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    if (consume(CHTLJSTokenType::LEFT_BRACE)) {
        std::string scriptContent;
        
        while (current_.getType() != CHTLJSTokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == CHTLJSTokenType::ENHANCED_SELECTOR_START) {
                auto selector = parseEnhancedSelector();
                if (selector) {
                    script->addChild(selector);
                }
            } else {
                scriptContent += current_.getValue();
                current_ = lexer_.nextToken();
            }
        }
        
        script->setAttribute("content", scriptContent);
        
        if (!consume(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return script;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnhancedSelector() {
    if (!consume(CHTLJSTokenType::ENHANCED_SELECTOR_START)) {
        reportError("Expected enhanced selector");
        return nullptr;
    }
    
    std::string selector = current_.getValue();
    current_ = lexer_.nextToken();
    
    return std::make_shared<EnhancedSelectorNode>(selector, current_.getLine(), current_.getColumn());
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    if (!consume(CHTLJSTokenType::ANIMATE)) {
        reportError("Expected animate");
        return nullptr;
    }
    
    auto animate = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    if (consume(CHTLJSTokenType::LEFT_BRACE)) {
        while (current_.getType() != CHTLJSTokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == CHTLJSTokenType::IDENTIFIER) {
                std::string key = current_.getValue();
                current_ = lexer_.nextToken();
                
                if (consume(CHTLJSTokenType::COLON)) {
                    std::string value;
                    if (current_.getType() == CHTLJSTokenType::STRING_LITERAL ||
                        current_.getType() == CHTLJSTokenType::UNQUOTED_LITERAL) {
                        value = current_.getValue();
                        current_ = lexer_.nextToken();
                    }
                    animate->setAttribute(key, value);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return animate;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListen() {
    if (!consume(CHTLJSTokenType::LISTEN)) {
        reportError("Expected listen");
        return nullptr;
    }
    
    auto listen = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    if (consume(CHTLJSTokenType::LEFT_BRACE)) {
        while (current_.getType() != CHTLJSTokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == CHTLJSTokenType::IDENTIFIER) {
                std::string event = current_.getValue();
                current_ = lexer_.nextToken();
                
                if (consume(CHTLJSTokenType::COLON)) {
                    // 解析事件处理函数
                    if (current_.getType() == CHTLJSTokenType::LEFT_BRACE) {
                        auto handler = parseScript();
                        if (handler) {
                            listen->addChild(handler);
                        }
                    }
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return listen;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUtil() {
    if (!consume(CHTLJSTokenType::UTIL)) {
        reportError("Expected util");
        return nullptr;
    }
    
    auto util = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    // 解析 util 表达式
    auto expression = parseExpression();
    if (expression) {
        util->addChild(expression);
    }
    
    if (consume(CHTLJSTokenType::ARROW)) {
        if (current_.getType() == CHTLJSTokenType::CHANGE) {
            current_ = lexer_.nextToken();
            auto changeBlock = parseScript();
            if (changeBlock) {
                util->addChild(changeBlock);
            }
        }
        
        if (consume(CHTLJSTokenType::ARROW)) {
            if (current_.getType() == CHTLJSTokenType::THEN) {
                current_ = lexer_.nextToken();
                auto thenBlock = parseScript();
                if (thenBlock) {
                    util->addChild(thenBlock);
                }
            }
        }
    }
    
    return util;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpression() {
    // 简化的表达式解析
    auto expression = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    std::string expr;
    while (current_.getType() != CHTLJSTokenType::ARROW && 
           current_.getType() != CHTLJSTokenType::SEMICOLON &&
           current_.getType() != CHTLJSTokenType::END_OF_FILE) {
        expr += current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    expression->setAttribute("expression", expr);
    return expression;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    // 简化的语句解析
    auto statement = std::make_shared<StatementNode>("", current_.getLine(), current_.getColumn());
    
    std::string stmt;
    while (current_.getType() != CHTLJSTokenType::SEMICOLON &&
           current_.getType() != CHTLJSTokenType::END_OF_FILE) {
        stmt += current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    statement->setAttribute("statement", stmt);
    return statement;
}

void CHTLJSParser::reportError(const std::string& message) {
    hasError_ = true;
    std::ostringstream oss;
    oss << "Error at line " << current_.getLine() 
        << ", column " << current_.getColumn() << ": " << message;
    errorMessage_ = oss.str();
}

void CHTLJSParser::synchronize() {
    while (current_.getType() != CHTLJSTokenType::END_OF_FILE) {
        if (current_.getType() == CHTLJSTokenType::SEMICOLON) {
            current_ = lexer_.nextToken();
            break;
        }
        current_ = lexer_.nextToken();
    }
}

} // namespace CHTL