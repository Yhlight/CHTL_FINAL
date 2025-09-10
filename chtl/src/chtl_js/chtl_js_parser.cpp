#include "chtl_js/chtl_js_parser.h"
#include <iostream>
#include <algorithm>

namespace chtl {
namespace chtl_js {

CHTLJSParser::CHTLJSParser(const std::string& source) 
    : current_(0), had_error_(false) {
    CHTLJSLexer lexer(source);
    tokens_ = lexer.tokenize();
}

CHTLJSParser::CHTLJSParser(const std::vector<CHTLJSToken>& tokens) 
    : tokens_(tokens), current_(0), had_error_(false) {}

std::shared_ptr<ProgramNode> CHTLJSParser::parse() {
    auto program = std::make_shared<ProgramNode>();
    
    while (!is_at_end()) {
        try {
            auto statement = parse_statement();
            if (statement) {
                program->body.push_back(statement);
            }
        } catch (const std::exception& e) {
            error(e.what());
            synchronize();
        }
    }
    
    return program;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_statement() {
    if (is_at_end()) return nullptr;
    
    CHTLJSToken token = current_token();
    
    switch (token.type) {
        case CHTLJSTokenType::VAR:
        case CHTLJSTokenType::LET:
        case CHTLJSTokenType::CONST:
            return parse_variable_declaration();
            
        case CHTLJSTokenType::FUNCTION:
            return parse_function_declaration();
            
        case CHTLJSTokenType::IF:
            return parse_if_statement();
            
        case CHTLJSTokenType::FOR:
            return parse_for_statement();
            
        case CHTLJSTokenType::WHILE:
            return parse_while_statement();
            
        case CHTLJSTokenType::SWITCH:
            return parse_switch_statement();
            
        case CHTLJSTokenType::TRY:
            return parse_try_statement();
            
        case CHTLJSTokenType::LEFT_BRACE:
            return parse_block_statement();
            
        case CHTLJSTokenType::FILELOADER:
            return parse_fileloader();
            
        case CHTLJSTokenType::LISTEN:
            return parse_listen();
            
        case CHTLJSTokenType::ANIMATE:
            return parse_animate();
            
        case CHTLJSTokenType::VIR:
            return parse_vir();
            
        case CHTLJSTokenType::ROUTER:
            return parse_router();
            
        case CHTLJSTokenType::INEVERAWAY:
            return parse_ineveraway();
            
        case CHTLJSTokenType::UTIL:
            return parse_util();
            
        default:
            // 如果遇到无法识别的token，尝试解析表达式
            if (is_expression_start()) {
                return parse_expression_statement();
            } else {
                error_at_current("Unexpected token");
                // 跳过当前token以避免无限循环
                advance();
                return nullptr;
            }
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_expression() {
    return parse_assignment_expression();
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_primary_expression() {
    if (is_at_end()) {
        error("Unexpected end of input");
        return nullptr;
    }
    
    CHTLJSToken token = current_token();
    
    switch (token.type) {
        case CHTLJSTokenType::IDENTIFIER:
            advance();
            return std::make_shared<IdentifierNode>(token.value, token.line, token.column);
            
        case CHTLJSTokenType::STRING_LITERAL:
        case CHTLJSTokenType::NUMBER_LITERAL:
        case CHTLJSTokenType::BOOLEAN_LITERAL:
            return parse_literal();
            
        case CHTLJSTokenType::LEFT_PAREN: {
            advance(); // 跳过 (
            auto expr = parse_expression();
            consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after expression");
            return expr;
        }
            
        case CHTLJSTokenType::LEFT_BRACKET:
            return parse_array_expression();
            
        case CHTLJSTokenType::LEFT_BRACE:
            return parse_object_expression();
            
        case CHTLJSTokenType::SELECTOR_START:
            return parse_selector();
            
        default:
            error_at_current("Expect expression");
            return nullptr;
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_binary_expression() {
    auto left = parse_primary_expression();
    
    while (match_any_token({
        CHTLJSTokenType::PLUS, CHTLJSTokenType::MINUS, CHTLJSTokenType::MULTIPLY,
        CHTLJSTokenType::DIVIDE, CHTLJSTokenType::MODULO, CHTLJSTokenType::POWER,
        CHTLJSTokenType::EQUAL, CHTLJSTokenType::NOT_EQUAL, CHTLJSTokenType::LESS,
        CHTLJSTokenType::LESS_EQUAL, CHTLJSTokenType::GREATER, CHTLJSTokenType::GREATER_EQUAL,
        CHTLJSTokenType::AND, CHTLJSTokenType::OR
    })) {
        CHTLJSToken operator_token = advance();
        auto right = parse_primary_expression();
        
        left = std::make_shared<BinaryExpressionNode>(
            left, operator_token.type, right, operator_token.line, operator_token.column);
    }
    
    return left;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_call_expression() {
    auto callee = parse_primary_expression();
    
    while (match_token(CHTLJSTokenType::LEFT_PAREN)) {
        advance(); // 跳过 (
        auto arguments = parse_expression_list();
        consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after arguments");
        
        callee = std::make_shared<CallExpressionNode>(callee, arguments, 0, 0);
    }
    
    return callee;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_member_expression() {
    auto object = parse_primary_expression();
    
    while (match_token(CHTLJSTokenType::DOT) || match_token(CHTLJSTokenType::ARROW) || 
           match_token(CHTLJSTokenType::LEFT_BRACKET)) {
        if (match_token(CHTLJSTokenType::DOT) || match_token(CHTLJSTokenType::ARROW)) {
            advance(); // 跳过 . 或 ->
            auto property = parse_primary_expression();
            object = std::make_shared<MemberExpressionNode>(object, property, false);
        } else {
            advance(); // 跳过 [
            auto property = parse_expression();
            consume_token(CHTLJSTokenType::RIGHT_BRACKET, "Expect ']' after property");
            object = std::make_shared<MemberExpressionNode>(object, property, true);
        }
    }
    
    return object;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_assignment_expression() {
    auto expr = parse_conditional_expression();
    
    if (match_token(CHTLJSTokenType::ASSIGN)) {
        CHTLJSToken equals = advance();
        auto value = parse_assignment_expression();
        
        if (expr->type == CHTLJSASTNodeType::IDENTIFIER) {
            auto name = std::static_pointer_cast<IdentifierNode>(expr)->name;
            return std::make_shared<BinaryExpressionNode>(
                expr, CHTLJSTokenType::ASSIGN, value, equals.line, equals.column);
        }
        
        error_at_token(equals, "Invalid assignment target");
    }
    
    return expr;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_conditional_expression() {
    auto expr = parse_binary_expression();
    
    if (match_token(CHTLJSTokenType::QUESTION)) {
        advance(); // 跳过 ?
        auto consequent = parse_expression();
        consume_token(CHTLJSTokenType::COLON, "Expect ':' after '?'");
        auto alternate = parse_expression();
        
        return std::make_shared<BinaryExpressionNode>(
            expr, CHTLJSTokenType::QUESTION, 
            std::make_shared<BinaryExpressionNode>(consequent, CHTLJSTokenType::COLON, alternate));
    }
    
    return expr;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_variable_declaration() {
    CHTLJSToken kind = advance(); // 跳过 var/let/const
    
    std::vector<std::shared_ptr<CHTLJSASTNode>> declarations;
    
    do {
        auto id = parse_primary_expression();
        std::shared_ptr<CHTLJSASTNode> init = nullptr;
        
        if (match_token(CHTLJSTokenType::ASSIGN)) {
            advance(); // 跳过 =
            init = parse_expression();
        }
        
        declarations.push_back(std::make_shared<VariableDeclaratorNode>(id, init));
    } while (match_token(CHTLJSTokenType::COMMA) && (advance(), true));
    
    consume_token(CHTLJSTokenType::SEMICOLON, "Expect ';' after variable declaration");
    
    return std::make_shared<VariableDeclarationNode>(kind.type, declarations);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_function_declaration() {
    advance(); // 跳过 function
    
    auto id = parse_primary_expression();
    consume_token(CHTLJSTokenType::LEFT_PAREN, "Expect '(' after function name");
    
    auto params = parse_parameter_list();
    consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after parameters");
    
    auto body = parse_block_statement();
    
    return std::make_shared<FunctionDeclarationNode>(id, params, body);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_class_declaration() {
    advance(); // 跳过 class
    
    auto id = parse_primary_expression();
    auto body = parse_block_statement();
    
    return std::make_shared<FunctionDeclarationNode>(id, std::vector<std::shared_ptr<CHTLJSASTNode>>(), body);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_if_statement() {
    advance(); // 跳过 if
    consume_token(CHTLJSTokenType::LEFT_PAREN, "Expect '(' after 'if'");
    
    auto condition = parse_expression();
    consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after condition");
    
    auto consequent = parse_statement();
    std::shared_ptr<CHTLJSASTNode> alternate = nullptr;
    
    if (match_token(CHTLJSTokenType::ELSE)) {
        advance(); // 跳过 else
        alternate = parse_statement();
    }
    
    return std::make_shared<BinaryExpressionNode>(condition, CHTLJSTokenType::IF, 
        std::make_shared<BinaryExpressionNode>(consequent, CHTLJSTokenType::ELSE, alternate));
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_for_statement() {
    advance(); // 跳过 for
    consume_token(CHTLJSTokenType::LEFT_PAREN, "Expect '(' after 'for'");
    
    auto init = parse_statement();
    auto condition = parse_expression();
    consume_token(CHTLJSTokenType::SEMICOLON, "Expect ';' after condition");
    auto update = parse_expression();
    consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after for clause");
    
    auto body = parse_statement();
    
    return std::make_shared<BinaryExpressionNode>(init, CHTLJSTokenType::FOR,
        std::make_shared<BinaryExpressionNode>(condition, CHTLJSTokenType::SEMICOLON,
            std::make_shared<BinaryExpressionNode>(update, CHTLJSTokenType::RIGHT_PAREN, body)));
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_while_statement() {
    advance(); // 跳过 while
    consume_token(CHTLJSTokenType::LEFT_PAREN, "Expect '(' after 'while'");
    
    auto condition = parse_expression();
    consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after condition");
    
    auto body = parse_statement();
    
    return std::make_shared<BinaryExpressionNode>(condition, CHTLJSTokenType::WHILE, body);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_switch_statement() {
    advance(); // 跳过 switch
    consume_token(CHTLJSTokenType::LEFT_PAREN, "Expect '(' after 'switch'");
    
    auto discriminant = parse_expression();
    consume_token(CHTLJSTokenType::RIGHT_PAREN, "Expect ')' after discriminant");
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after switch");
    
    auto body = parse_block_statement();
    
    return std::make_shared<BinaryExpressionNode>(discriminant, CHTLJSTokenType::SWITCH, body);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_try_statement() {
    advance(); // 跳过 try
    
    auto block = parse_block_statement();
    std::shared_ptr<CHTLJSASTNode> handler = nullptr;
    std::shared_ptr<CHTLJSASTNode> finalizer = nullptr;
    
    if (match_token(CHTLJSTokenType::CATCH)) {
        advance(); // 跳过 catch
        handler = parse_block_statement();
    }
    
    if (match_token(CHTLJSTokenType::FINALLY)) {
        advance(); // 跳过 finally
        finalizer = parse_block_statement();
    }
    
    return std::make_shared<BinaryExpressionNode>(block, CHTLJSTokenType::TRY,
        std::make_shared<BinaryExpressionNode>(handler, CHTLJSTokenType::CATCH, finalizer));
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_block_statement() {
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{'");
    
    std::vector<std::shared_ptr<CHTLJSASTNode>> statements;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        auto statement = parse_statement();
        if (statement) {
            statements.push_back(statement);
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after block");
    
    return std::make_shared<BlockStatementNode>(statements);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_expression_statement() {
    auto expr = parse_expression();
    consume_token(CHTLJSTokenType::SEMICOLON, "Expect ';' after expression");
    return expr;
}

// CHTL JS特有语法解析
std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_fileloader() {
    advance(); // 跳过 fileloader
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after 'fileloader'");
    
    std::vector<std::string> load_files;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(CHTLJSTokenType::STRING_LITERAL)) {
            auto token = advance();
            load_files.push_back(token.value);
        } else if (match_token(CHTLJSTokenType::IDENTIFIER)) {
            auto token = advance();
            load_files.push_back(token.value);
        } else {
            advance();
        }
        
        if (match_token(CHTLJSTokenType::COMMA)) {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after fileloader");
    
    return std::make_shared<FileloaderNode>(load_files);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_listen() {
    advance(); // 跳过 listen
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after 'listen'");
    
    std::string event;
    std::shared_ptr<CHTLJSASTNode> selector = nullptr;
    std::shared_ptr<CHTLJSASTNode> handler = nullptr;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(CHTLJSTokenType::IDENTIFIER)) {
            auto token = advance();
            if (token.value == "event") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'event'");
                if (match_token(CHTLJSTokenType::STRING_LITERAL)) {
                    event = advance().value;
                }
            } else if (token.value == "selector") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'selector'");
                selector = parse_expression();
            } else if (token.value == "handler") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'handler'");
                handler = parse_expression();
            }
        } else {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after listen");
    
    return std::make_shared<ListenNode>(event, selector, handler);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_animate() {
    advance(); // 跳过 animate
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after 'animate'");
    
    auto animate = std::make_shared<AnimateNode>(nullptr, 100, "ease-in-out");
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(CHTLJSTokenType::IDENTIFIER)) {
            auto token = advance();
            if (token.value == "target") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'target'");
                animate->target = parse_expression();
            } else if (token.value == "duration") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'duration'");
                if (match_token(CHTLJSTokenType::NUMBER_LITERAL)) {
                    animate->duration = std::stoi(advance().value);
                }
            }
            // 可以继续添加其他属性
        } else {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after animate");
    
    return animate;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_vir() {
    advance(); // 跳过 vir
    
    auto name_token = advance(); // 获取变量名
    consume_token(CHTLJSTokenType::ASSIGN, "Expect '=' after vir name");
    
    auto value = parse_expression();
    
    return std::make_shared<VirNode>(name_token.value, value);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_router() {
    advance(); // 跳过 router
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after 'router'");
    
    std::string path;
    std::shared_ptr<CHTLJSASTNode> handler = nullptr;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(CHTLJSTokenType::IDENTIFIER)) {
            auto token = advance();
            if (token.value == "path") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'path'");
                if (match_token(CHTLJSTokenType::STRING_LITERAL)) {
                    path = advance().value;
                }
            } else if (token.value == "handler") {
                consume_token(CHTLJSTokenType::COLON, "Expect ':' after 'handler'");
                handler = parse_expression();
            }
        } else {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after router");
    
    return std::make_shared<RouterNode>(path, handler);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_ineveraway() {
    advance(); // 跳过 iNeverAway
    consume_token(CHTLJSTokenType::LEFT_BRACE, "Expect '{' after 'iNeverAway'");
    
    std::unordered_map<std::string, std::shared_ptr<CHTLJSASTNode>> functions;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        if (match_token(CHTLJSTokenType::IDENTIFIER)) {
            auto name_token = advance();
            consume_token(CHTLJSTokenType::COLON, "Expect ':' after function name");
            
            auto func = parse_expression();
            functions[name_token.value] = func;
        } else {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after iNeverAway");
    
    return std::make_shared<INeverAwayNode>(functions);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_util() {
    advance(); // 跳过 util
    
    auto condition = parse_expression();
    consume_token(CHTLJSTokenType::ARROW, "Expect '->' after condition");
    
    std::shared_ptr<CHTLJSASTNode> change = nullptr;
    std::shared_ptr<CHTLJSASTNode> then = nullptr;
    
    if (match_token(CHTLJSTokenType::CHANGE)) {
        advance(); // 跳过 change
        change = parse_expression();
        consume_token(CHTLJSTokenType::ARROW, "Expect '->' after change");
    }
    
    if (match_token(CHTLJSTokenType::THEN)) {
        advance(); // 跳过 then
        then = parse_expression();
    }
    
    return std::make_shared<UtilNode>(condition, change, then);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_selector() {
    auto token = advance(); // 跳过 {{
    
    std::string selector = token.value;
    bool is_self_ref = (selector == "{{&}}");
    
    return std::make_shared<SelectorNode>(selector, is_self_ref, token.line, token.column);
}

// 辅助方法
bool CHTLJSParser::match_token(CHTLJSTokenType type) {
    if (is_at_end()) return false;
    return current_token().type == type;
}

bool CHTLJSParser::match_any_token(const std::vector<CHTLJSTokenType>& types) {
    for (auto type : types) {
        if (match_token(type)) return true;
    }
    return false;
}

CHTLJSToken CHTLJSParser::consume_token(CHTLJSTokenType type, const std::string& error_message) {
    if (match_token(type)) return advance();
    
    error_at_current(error_message);
    return current_token();
}

CHTLJSToken CHTLJSParser::advance() {
    if (!is_at_end()) current_++;
    return current_token();
}

void CHTLJSParser::synchronize() {
    advance();
    
    while (!is_at_end()) {
        if (current_token().type == CHTLJSTokenType::SEMICOLON) {
            advance();
            break;
        }
        
        switch (current_token().type) {
            case CHTLJSTokenType::FUNCTION:
            case CHTLJSTokenType::VAR:
            case CHTLJSTokenType::LET:
            case CHTLJSTokenType::CONST:
            case CHTLJSTokenType::IF:
            case CHTLJSTokenType::FOR:
            case CHTLJSTokenType::WHILE:
            case CHTLJSTokenType::SWITCH:
            case CHTLJSTokenType::TRY:
            case CHTLJSTokenType::RETURN:
                return;
            default:
                advance();
                break;
        }
    }
}

void CHTLJSParser::error(const std::string& message) {
    had_error_ = true;
    last_error_ = message;
    std::cerr << "Parse Error: " << message << std::endl;
}

void CHTLJSParser::error_at_current(const std::string& message) {
    error_at_token(current_token(), message);
}

void CHTLJSParser::error_at_token(const CHTLJSToken& token, const std::string& message) {
    if (token.type == CHTLJSTokenType::EOF_TOKEN) {
        error("At end: " + message);
    } else {
        error("At '" + token.value + "': " + message);
    }
}

bool CHTLJSParser::is_at_end() const {
    return current_ >= tokens_.size() || 
           tokens_[current_].type == CHTLJSTokenType::EOF_TOKEN;
}

bool CHTLJSParser::is_expression_start() const {
    if (is_at_end()) return false;
    
    CHTLJSTokenType type = current_token().type;
    return type == CHTLJSTokenType::IDENTIFIER ||
           type == CHTLJSTokenType::NUMBER_LITERAL ||
           type == CHTLJSTokenType::STRING_LITERAL ||
           type == CHTLJSTokenType::BOOLEAN_LITERAL ||
           type == CHTLJSTokenType::LEFT_PAREN ||
           type == CHTLJSTokenType::LEFT_BRACKET ||
           type == CHTLJSTokenType::LEFT_BRACE ||
           type == CHTLJSTokenType::MINUS ||
           type == CHTLJSTokenType::NOT ||
           type == CHTLJSTokenType::PLUS ||
           type == CHTLJSTokenType::NEW ||
           type == CHTLJSTokenType::THIS ||
           type == CHTLJSTokenType::SUPER;
}

CHTLJSToken CHTLJSParser::current_token() const {
    if (is_at_end()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[current_];
}

CHTLJSToken CHTLJSParser::peek_token(size_t offset) const {
    if (current_ + offset >= tokens_.size()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[current_ + offset];
}

void CHTLJSParser::reset() {
    current_ = 0;
    had_error_ = false;
    last_error_.clear();
}

// 表达式解析辅助方法
std::vector<std::shared_ptr<CHTLJSASTNode>> CHTLJSParser::parse_expression_list() {
    std::vector<std::shared_ptr<CHTLJSASTNode>> expressions;
    
    if (!match_token(CHTLJSTokenType::RIGHT_PAREN)) {
        do {
            expressions.push_back(parse_expression());
        } while (match_token(CHTLJSTokenType::COMMA) && (advance(), true));
    }
    
    return expressions;
}

std::vector<std::shared_ptr<CHTLJSASTNode>> CHTLJSParser::parse_parameter_list() {
    std::vector<std::shared_ptr<CHTLJSASTNode>> parameters;
    
    if (!match_token(CHTLJSTokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(parse_primary_expression());
        } while (match_token(CHTLJSTokenType::COMMA) && (advance(), true));
    }
    
    return parameters;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_object_expression() {
    advance(); // 跳过 {
    
    std::vector<std::shared_ptr<CHTLJSASTNode>> properties;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        properties.push_back(parse_property());
        
        if (match_token(CHTLJSTokenType::COMMA)) {
            advance();
        }
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACE, "Expect '}' after object");
    
    return std::make_shared<BinaryExpressionNode>(
        nullptr, CHTLJSTokenType::LEFT_BRACE, nullptr); // 临时实现
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_array_expression() {
    advance(); // 跳过 [
    
    std::vector<std::shared_ptr<CHTLJSASTNode>> elements;
    
    if (!match_token(CHTLJSTokenType::RIGHT_BRACKET)) {
        do {
            elements.push_back(parse_expression());
        } while (match_token(CHTLJSTokenType::COMMA) && (advance(), true));
    }
    
    consume_token(CHTLJSTokenType::RIGHT_BRACKET, "Expect ']' after array");
    
    return std::make_shared<BinaryExpressionNode>(
        nullptr, CHTLJSTokenType::LEFT_BRACKET, nullptr); // 临时实现
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_literal() {
    CHTLJSToken token = advance();
    
    switch (token.type) {
        case CHTLJSTokenType::STRING_LITERAL:
            return std::make_shared<LiteralNode>(token.value, token.type, token.line, token.column);
        case CHTLJSTokenType::NUMBER_LITERAL:
            return std::make_shared<LiteralNode>(token.value, token.type, token.line, token.column);
        case CHTLJSTokenType::BOOLEAN_LITERAL:
            return std::make_shared<LiteralNode>(token.value, token.type, token.line, token.column);
        default:
            error_at_token(token, "Unexpected literal type");
            return nullptr;
    }
}

std::vector<std::shared_ptr<CHTLJSASTNode>> CHTLJSParser::parse_property_list() {
    std::vector<std::shared_ptr<CHTLJSASTNode>> properties;
    
    while (!match_token(CHTLJSTokenType::RIGHT_BRACE) && !is_at_end()) {
        properties.push_back(parse_property());
        
        if (match_token(CHTLJSTokenType::COMMA)) {
            advance();
        }
    }
    
    return properties;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse_property() {
    auto key = parse_primary_expression();
    
    if (match_token(CHTLJSTokenType::COLON)) {
        advance(); // 跳过 :
        auto value = parse_expression();
        return std::make_shared<BinaryExpressionNode>(key, CHTLJSTokenType::COLON, value);
    }
    
    return key;
}

std::string CHTLJSParser::parse_selector_string() {
    if (is_selector_start()) {
        auto token = advance();
        return token.value;
    }
    return "";
}

bool CHTLJSParser::is_selector_start() {
    return match_token(CHTLJSTokenType::SELECTOR_START);
}

} // namespace chtl_js
} // namespace chtl