#include "CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

void CHTLLexer::addError(const std::string& message) {
    std::ostringstream oss;
    oss << "第" << line << "行第" << column << "列: " << message;
    errors.push_back(oss.str());
}

CHTLToken CHTLLexer::createErrorToken(const std::string& message) {
    addError(message);
    return CHTLToken(CHTLTokenType::ERROR, message, line, column, position);
}

std::vector<CHTLToken> CHTLLexer::tokenize() {
    tokens.clear();
    errors.clear();
    position = 0;
    line = 1;
    column = 1;
    
    while (position < source.length()) {
        skipWhitespace();
        
        if (position >= source.length()) break;
        
        char c = current();
        
        if (isAlpha(c)) {
            tokens.push_back(scanIdentifier());
        } else if (c == '"' || c == '\'') {
            tokens.push_back(scanString());
        } else if (isDigit(c)) {
            tokens.push_back(scanNumber());
        } else if (c == '/' && (peek() == '/' || peek() == '*')) {
            tokens.push_back(scanComment());
        } else if (c == '-' && peek() == '-') {
            tokens.push_back(scanComment());
        } else if (c == '[') {
            tokens.push_back(scanTemplateKeyword());
        } else if (c == '@') {
            tokens.push_back(scanOriginKeyword());
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || 
                   c == '&' || c == '|' || c == '!' || c == '<' || c == '>' || c == '=') {
            tokens.push_back(scanOperator());
        } else if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
                   c == ';' || c == ':' || c == '=' || c == ',' || c == '.' || c == '&' ||
                   c == '?' || c == '|' || c == '@' || c == '#' || c == '$' || c == '~' || c == '^') {
            tokens.push_back(scanSymbol());
        } else if (!isWhitespace(c) && c != '\n') {
            // 尝试作为无引号字面量
            tokens.push_back(scanUnquotedLiteral());
        } else {
            advance();
        }
    }
    
    // 添加EOF token
    tokens.push_back(CHTLToken(CHTLTokenType::EOF_TOKEN, "", line, column, position));
    
    return tokens;
}

std::vector<CHTLToken> CHTLLexer::tokenize(const std::string& source) {
    setSource(source);
    return tokenize();
}

bool CHTLLexer::hasErrors() const {
    return !errors.empty();
}

std::vector<std::string> CHTLLexer::getErrors() const {
    return errors;
}

size_t CHTLLexer::getTokenCount() const {
    return tokens.size();
}

size_t CHTLLexer::getCurrentPosition() const {
    return position;
}

void CHTLLexer::printTokens() const {
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

void CHTLLexer::printErrors() const {
    for (const auto& error : errors) {
        std::cout << "错误: " << error << std::endl;
    }
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
    errors.clear();
}

void CHTLLexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

// CHTLToken方法实现
bool CHTLToken::isKeyword() const {
    return type >= CHTLTokenType::TEXT && type <= CHTLTokenType::ROUTER;
}

bool CHTLToken::isOperator() const {
    return type >= CHTLTokenType::PLUS && type <= CHTLTokenType::NOT_EQUAL;
}

bool CHTLToken::isSymbol() const {
    return type >= CHTLTokenType::LEFT_BRACE && type <= CHTLTokenType::CARET;
}

bool CHTLToken::isComment() const {
    return type == CHTLTokenType::LINE_COMMENT || 
           type == CHTLTokenType::BLOCK_COMMENT || 
           type == CHTLTokenType::GENERATOR_COMMENT;
}

std::string CHTLToken::toString() const {
    std::ostringstream oss;
    oss << "Token(" << tokenTypeToString(type) << ", \"" << value << "\", " 
         << line << ":" << column << ")";
    return oss.str();
}

// 辅助函数实现
std::string tokenTypeToString(CHTLTokenType type) {
    switch (type) {
        case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLTokenType::STRING: return "STRING";
        case CHTLTokenType::NUMBER: return "NUMBER";
        case CHTLTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLTokenType::COLON: return "COLON";
        case CHTLTokenType::EQUAL: return "EQUAL";
        case CHTLTokenType::COMMA: return "COMMA";
        case CHTLTokenType::DOT: return "DOT";
        case CHTLTokenType::ARROW: return "ARROW";
        case CHTLTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLTokenType::QUESTION: return "QUESTION";
        case CHTLTokenType::PIPE: return "PIPE";
        case CHTLTokenType::PLUS: return "PLUS";
        case CHTLTokenType::MINUS: return "MINUS";
        case CHTLTokenType::MULTIPLY: return "MULTIPLY";
        case CHTLTokenType::DIVIDE: return "DIVIDE";
        case CHTLTokenType::MODULO: return "MODULO";
        case CHTLTokenType::POWER: return "POWER";
        case CHTLTokenType::AND: return "AND";
        case CHTLTokenType::OR: return "OR";
        case CHTLTokenType::NOT: return "NOT";
        case CHTLTokenType::LESS: return "LESS";
        case CHTLTokenType::GREATER: return "GREATER";
        case CHTLTokenType::LESS_EQUAL: return "LESS_EQUAL";
        case CHTLTokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case CHTLTokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case CHTLTokenType::NOT_EQUAL: return "NOT_EQUAL";
        case CHTLTokenType::LINE_COMMENT: return "LINE_COMMENT";
        case CHTLTokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case CHTLTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLTokenType::TEXT: return "TEXT";
        case CHTLTokenType::STYLE: return "STYLE";
        case CHTLTokenType::SCRIPT: return "SCRIPT";
        case CHTLTokenType::TEMPLATE: return "TEMPLATE";
        case CHTLTokenType::CUSTOM: return "CUSTOM";
        case CHTLTokenType::ORIGIN: return "ORIGIN";
        case CHTLTokenType::IMPORT: return "IMPORT";
        case CHTLTokenType::NAMESPACE: return "NAMESPACE";
        case CHTLTokenType::CONSTRAINT: return "CONSTRAINT";
        case CHTLTokenType::CONFIGURATION: return "CONFIGURATION";
        case CHTLTokenType::USE: return "USE";
        case CHTLTokenType::FILELOADER: return "FILELOADER";
        case CHTLTokenType::LISTEN: return "LISTEN";
        case CHTLTokenType::DELEGATE: return "DELEGATE";
        case CHTLTokenType::ANIMATE: return "ANIMATE";
        case CHTLTokenType::VIR: return "VIR";
        case CHTLTokenType::ROUTER: return "ROUTER";
        case CHTLTokenType::AT: return "AT";
        case CHTLTokenType::HASH: return "HASH";
        case CHTLTokenType::DOLLAR: return "DOLLAR";
        case CHTLTokenType::TILDE: return "TILDE";
        case CHTLTokenType::CARET: return "CARET";
        case CHTLTokenType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case CHTLTokenType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case CHTLTokenType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case CHTLTokenType::INHERIT: return "INHERIT";
        case CHTLTokenType::DELETE: return "DELETE";
        case CHTLTokenType::INSERT: return "INSERT";
        case CHTLTokenType::EOF_TOKEN: return "EOF_TOKEN";
        case CHTLTokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool isHTMLElement(const std::string& name) {
    static std::set<std::string> elements = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option",
        "header", "footer", "nav", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "code", "pre", "blockquote"
    };
    return elements.find(name) != elements.end();
}

bool isCHTLKeyword(const std::string& name) {
    static std::set<std::string> keywords = {
        "text", "style", "script", "template", "custom", "import", 
        "namespace", "constraint", "configuration", "use"
    };
    return keywords.find(name) != keywords.end();
}

bool isCHTLJSKeyword(const std::string& name) {
    static std::set<std::string> keywords = {
        "fileloader", "listen", "delegate", "animate", "vir", "router"
    };
    return keywords.find(name) != keywords.end();
}

} // namespace CHTL