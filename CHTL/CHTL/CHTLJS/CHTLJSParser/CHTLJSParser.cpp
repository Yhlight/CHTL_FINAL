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
    // 完整的表达式解析
    return parseLogicalOrExpression();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseLogicalOrExpression() {
    auto left = parseLogicalAndExpression();
    
    while (current_.getType() == CHTLJSTokenType::LOGICAL_OR) {
        current_ = lexer_.nextToken();
        auto right = parseLogicalAndExpression();
        
        auto logicalOr = std::make_shared<StatementNode>("logicalOr", current_.getLine(), current_.getColumn());
        logicalOr->addChild(left);
        logicalOr->addChild(right);
        left = logicalOr;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseLogicalAndExpression() {
    auto left = parseEqualityExpression();
    
    while (current_.getType() == CHTLJSTokenType::LOGICAL_AND) {
        current_ = lexer_.nextToken();
        auto right = parseEqualityExpression();
        
        auto logicalAnd = std::make_shared<StatementNode>("logicalAnd", current_.getLine(), current_.getColumn());
        logicalAnd->addChild(left);
        logicalAnd->addChild(right);
        left = logicalAnd;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEqualityExpression() {
    auto left = parseRelationalExpression();
    
    while (current_.getType() == CHTLJSTokenType::EQUAL_EQUAL || 
           current_.getType() == CHTLJSTokenType::NOT_EQUAL) {
        std::string op = current_.getValue();
        current_ = lexer_.nextToken();
        auto right = parseRelationalExpression();
        
        auto equality = std::make_shared<StatementNode>("equality", current_.getLine(), current_.getColumn());
        equality->setAttribute("operator", op);
        equality->addChild(left);
        equality->addChild(right);
        left = equality;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRelationalExpression() {
    auto left = parseAdditiveExpression();
    
    while (current_.getType() == CHTLJSTokenType::LESS_THAN ||
           current_.getType() == CHTLJSTokenType::GREATER_THAN ||
           current_.getType() == CHTLJSTokenType::LESS_EQUAL ||
           current_.getType() == CHTLJSTokenType::GREATER_EQUAL) {
        std::string op = current_.getValue();
        current_ = lexer_.nextToken();
        auto right = parseAdditiveExpression();
        
        auto relational = std::make_shared<StatementNode>("relational", current_.getLine(), current_.getColumn());
        relational->setAttribute("operator", op);
        relational->addChild(left);
        relational->addChild(right);
        left = relational;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();
    
    while (current_.getType() == CHTLJSTokenType::PLUS ||
           current_.getType() == CHTLJSTokenType::MINUS) {
        std::string op = current_.getValue();
        current_ = lexer_.nextToken();
        auto right = parseMultiplicativeExpression();
        
        auto additive = std::make_shared<StatementNode>("additive", current_.getLine(), current_.getColumn());
        additive->setAttribute("operator", op);
        additive->addChild(left);
        additive->addChild(right);
        left = additive;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();
    
    while (current_.getType() == CHTLJSTokenType::MULTIPLY ||
           current_.getType() == CHTLJSTokenType::DIVIDE ||
           current_.getType() == CHTLJSTokenType::MODULO) {
        std::string op = current_.getValue();
        current_ = lexer_.nextToken();
        auto right = parseUnaryExpression();
        
        auto multiplicative = std::make_shared<StatementNode>("multiplicative", current_.getLine(), current_.getColumn());
        multiplicative->setAttribute("operator", op);
        multiplicative->addChild(left);
        multiplicative->addChild(right);
        left = multiplicative;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUnaryExpression() {
    if (current_.getType() == CHTLJSTokenType::MINUS ||
        current_.getType() == CHTLJSTokenType::LOGICAL_NOT ||
        current_.getType() == CHTLJSTokenType::PLUS_PLUS ||
        current_.getType() == CHTLJSTokenType::MINUS_MINUS) {
        std::string op = current_.getValue();
        current_ = lexer_.nextToken();
        auto operand = parsePrimaryExpression();
        
        auto unary = std::make_shared<StatementNode>("unary", current_.getLine(), current_.getColumn());
        unary->setAttribute("operator", op);
        unary->addChild(operand);
        return unary;
    }
    
    return parsePrimaryExpression();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrimaryExpression() {
    if (current_.getType() == CHTLJSTokenType::IDENTIFIER) {
        auto identifier = std::make_shared<StatementNode>("identifier", current_.getLine(), current_.getColumn());
        identifier->setAttribute("name", current_.getValue());
        current_ = lexer_.nextToken();
        return identifier;
    }
    
    if (current_.getType() == CHTLJSTokenType::NUMBER_LITERAL) {
        auto number = std::make_shared<StatementNode>("number", current_.getLine(), current_.getColumn());
        number->setAttribute("value", current_.getValue());
        current_ = lexer_.nextToken();
        return number;
    }
    
    if (current_.getType() == CHTLJSTokenType::STRING_LITERAL) {
        auto string = std::make_shared<StatementNode>("string", current_.getLine(), current_.getColumn());
        string->setAttribute("value", current_.getValue());
        current_ = lexer_.nextToken();
        return string;
    }
    
    if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
        current_ = lexer_.nextToken();
        auto expr = parseExpression();
        if (current_.getType() != CHTLJSTokenType::RIGHT_PAREN) {
            // 错误处理
            return expr;
        }
        current_ = lexer_.nextToken();
        return expr;
    }
    
    // 函数调用
    if (current_.getType() == CHTLJSTokenType::IDENTIFIER) {
        std::string functionName = current_.getValue();
        current_ = lexer_.nextToken();
        
        if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
            current_ = lexer_.nextToken();
            auto call = std::make_shared<StatementNode>("functionCall", current_.getLine(), current_.getColumn());
            call->setAttribute("name", functionName);
            
            // 解析参数
            while (current_.getType() != CHTLJSTokenType::RIGHT_PAREN && 
                   current_.getType() != CHTLJSTokenType::END_OF_FILE) {
                auto arg = parseExpression();
                call->addChild(arg);
                
                if (current_.getType() == CHTLJSTokenType::COMMA) {
                    current_ = lexer_.nextToken();
                }
            }
            
            if (current_.getType() == CHTLJSTokenType::RIGHT_PAREN) {
                current_ = lexer_.nextToken();
            }
            
            return call;
        }
    }
    
    // 默认返回空表达式
    return std::make_shared<StatementNode>("empty", current_.getLine(), current_.getColumn());
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    // 完整的语句解析
    if (current_.getType() == CHTLJSTokenType::END_OF_FILE) {
        return nullptr;
    }
    
    // 变量声明
    if (current_.getType() == CHTLJSTokenType::VAR || 
        current_.getType() == CHTLJSTokenType::LET || 
        current_.getType() == CHTLJSTokenType::CONST) {
        return parseVariableDeclaration();
    }
    
    // 函数声明
    if (current_.getType() == CHTLJSTokenType::FUNCTION) {
        return parseFunctionDeclaration();
    }
    
    // if 语句
    if (current_.getType() == CHTLJSTokenType::IF) {
        return parseIfStatement();
    }
    
    // while 循环
    if (current_.getType() == CHTLJSTokenType::WHILE) {
        return parseWhileStatement();
    }
    
    // for 循环
    if (current_.getType() == CHTLJSTokenType::FOR) {
        return parseForStatement();
    }
    
    // return 语句
    if (current_.getType() == CHTLJSTokenType::RETURN) {
        return parseReturnStatement();
    }
    
    // 表达式语句
    auto expr = parseExpression();
    
    // 检查分号
    if (current_.getType() == CHTLJSTokenType::SEMICOLON) {
        current_ = lexer_.nextToken();
    }
    
    return expr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVariableDeclaration() {
    std::string keyword = current_.getValue();
    current_ = lexer_.nextToken();
    
    if (current_.getType() != CHTLJSTokenType::IDENTIFIER) {
        // 错误处理
        return std::make_shared<StatementNode>("error", current_.getLine(), current_.getColumn());
    }
    
    std::string varName = current_.getValue();
    current_ = lexer_.nextToken();
    
    auto declaration = std::make_shared<StatementNode>("variableDeclaration", current_.getLine(), current_.getColumn());
    declaration->setAttribute("keyword", keyword);
    declaration->setAttribute("name", varName);
    
    // 检查是否有赋值
    if (current_.getType() == CHTLJSTokenType::ASSIGN) {
        current_ = lexer_.nextToken();
        auto value = parseExpression();
        declaration->addChild(value);
    }
    
    return declaration;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionDeclaration() {
    current_ = lexer_.nextToken(); // 跳过 function
    
    if (current_.getType() != CHTLJSTokenType::IDENTIFIER) {
        // 错误处理
        return std::make_shared<StatementNode>("error", current_.getLine(), current_.getColumn());
    }
    
    std::string functionName = current_.getValue();
    current_ = lexer_.nextToken();
    
    auto function = std::make_shared<StatementNode>("functionDeclaration", current_.getLine(), current_.getColumn());
    function->setAttribute("name", functionName);
    
    // 解析参数
    if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
        current_ = lexer_.nextToken();
        
        while (current_.getType() != CHTLJSTokenType::RIGHT_PAREN && 
               current_.getType() != CHTLJSTokenType::END_OF_FILE) {
            if (current_.getType() == CHTLJSTokenType::IDENTIFIER) {
                auto param = std::make_shared<StatementNode>("parameter", current_.getLine(), current_.getColumn());
                param->setAttribute("name", current_.getValue());
                function->addChild(param);
                current_ = lexer_.nextToken();
            }
            
            if (current_.getType() == CHTLJSTokenType::COMMA) {
                current_ = lexer_.nextToken();
            }
        }
        
        if (current_.getType() == CHTLJSTokenType::RIGHT_PAREN) {
            current_ = lexer_.nextToken();
        }
    }
    
    // 解析函数体
    if (current_.getType() == CHTLJSTokenType::LEFT_BRACE) {
        current_ = lexer_.nextToken();
        
        while (current_.getType() != CHTLJSTokenType::RIGHT_BRACE && 
               current_.getType() != CHTLJSTokenType::END_OF_FILE) {
            auto stmt = parseStatement();
            if (stmt) {
                function->addChild(stmt);
            }
        }
        
        if (current_.getType() == CHTLJSTokenType::RIGHT_BRACE) {
            current_ = lexer_.nextToken();
        }
    }
    
    return function;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseIfStatement() {
    current_ = lexer_.nextToken(); // 跳过 if
    
    auto ifStmt = std::make_shared<StatementNode>("ifStatement", current_.getLine(), current_.getColumn());
    
    // 解析条件
    if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
        current_ = lexer_.nextToken();
        auto condition = parseExpression();
        ifStmt->addChild(condition);
        
        if (current_.getType() == CHTLJSTokenType::RIGHT_PAREN) {
            current_ = lexer_.nextToken();
        }
    }
    
    // 解析 then 分支
    auto thenBranch = parseStatement();
    if (thenBranch) {
        ifStmt->addChild(thenBranch);
    }
    
    // 解析 else 分支
    if (current_.getType() == CHTLJSTokenType::ELSE) {
        current_ = lexer_.nextToken();
        auto elseBranch = parseStatement();
        if (elseBranch) {
            ifStmt->addChild(elseBranch);
        }
    }
    
    return ifStmt;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseWhileStatement() {
    current_ = lexer_.nextToken(); // 跳过 while
    
    auto whileStmt = std::make_shared<StatementNode>("whileStatement", current_.getLine(), current_.getColumn());
    
    // 解析条件
    if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
        current_ = lexer_.nextToken();
        auto condition = parseExpression();
        whileStmt->addChild(condition);
        
        if (current_.getType() == CHTLJSTokenType::RIGHT_PAREN) {
            current_ = lexer_.nextToken();
        }
    }
    
    // 解析循环体
    auto body = parseStatement();
    if (body) {
        whileStmt->addChild(body);
    }
    
    return whileStmt;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseForStatement() {
    current_ = lexer_.nextToken(); // 跳过 for
    
    auto forStmt = std::make_shared<StatementNode>("forStatement", current_.getLine(), current_.getColumn());
    
    if (current_.getType() == CHTLJSTokenType::LEFT_PAREN) {
        current_ = lexer_.nextToken();
        
        // 初始化
        if (current_.getType() != CHTLJSTokenType::SEMICOLON) {
            auto init = parseStatement();
            if (init) {
                forStmt->addChild(init);
            }
        }
        
        // 条件
        if (current_.getType() == CHTLJSTokenType::SEMICOLON) {
            current_ = lexer_.nextToken();
            if (current_.getType() != CHTLJSTokenType::SEMICOLON) {
                auto condition = parseExpression();
                forStmt->addChild(condition);
            }
        }
        
        // 更新
        if (current_.getType() == CHTLJSTokenType::SEMICOLON) {
            current_ = lexer_.nextToken();
            if (current_.getType() != CHTLJSTokenType::RIGHT_PAREN) {
                auto update = parseExpression();
                forStmt->addChild(update);
            }
        }
        
        if (current_.getType() == CHTLJSTokenType::RIGHT_PAREN) {
            current_ = lexer_.nextToken();
        }
    }
    
    // 循环体
    auto body = parseStatement();
    if (body) {
        forStmt->addChild(body);
    }
    
    return forStmt;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseReturnStatement() {
    current_ = lexer_.nextToken(); // 跳过 return
    
    auto returnStmt = std::make_shared<StatementNode>("returnStatement", current_.getLine(), current_.getColumn());
    
    // 解析返回值
    if (current_.getType() != CHTLJSTokenType::SEMICOLON && 
        current_.getType() != CHTLJSTokenType::END_OF_FILE) {
        auto value = parseExpression();
        returnStmt->addChild(value);
    }
    
    return returnStmt;
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